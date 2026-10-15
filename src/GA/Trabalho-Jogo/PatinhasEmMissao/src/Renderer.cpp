#include "Renderer.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
const glm::vec4 INK{.13f,.27f,.24f,1};
const glm::vec4 MUTED{.32f,.43f,.36f,1};
const glm::vec4 PAPER{.99f,.97f,.89f,1};
const glm::vec4 TEAL{.16f,.43f,.37f,1};
const glm::vec4 GOLD{.91f,.69f,.33f,1};

std::string readFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Arquivo ausente: " + path.string());
    std::ostringstream buffer; buffer << file.rdbuf(); return buffer.str();
}
GLuint compile(GLenum type, const std::string& source) {
    GLuint result = glCreateShader(type);
    const char* code = source.c_str();
    glShaderSource(result, 1, &code, nullptr);
    glCompileShader(result);
    GLint success; glGetShaderiv(result, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[2048]; glGetShaderInfoLog(result, sizeof(log), nullptr, log);
        glDeleteShader(result);
        throw std::runtime_error(std::string("Erro de shader: ") + log);
    }
    return result;
}
// O atlas cobre Latin-1, suficiente para os textos em português deste jogo.
// Converte o UTF-8 do código-fonte para pontos de código antes de recortar.
std::vector<unsigned> letters(const std::string& s) {
    std::vector<unsigned> result;
    for (size_t i = 0; i < s.size();) {
        unsigned c = static_cast<unsigned char>(s[i++]);
        if ((c & 0xE0) == 0xC0 && i < s.size())
            c = ((c & 0x1F) << 6) | (static_cast<unsigned char>(s[i++]) & 0x3F);
        else if (c >= 128) c = '?';
        result.push_back(c < 256 ? c : '?');
    }
    return result;
}
}

Renderer::Renderer(const std::filesystem::path& root, const Config& config) {
    GLuint vertex = compile(GL_VERTEX_SHADER, readFile(root / "shaders/sprite.vert"));
    GLuint fragment = compile(GL_FRAGMENT_SHADER, readFile(root / "shaders/sprite.frag"));
    shader = glCreateProgram();
    glAttachShader(shader, vertex); glAttachShader(shader, fragment);
    glLinkProgram(shader);
    glDeleteShader(vertex); glDeleteShader(fragment);
    GLint ok; glGetProgramiv(shader, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetProgramInfoLog(shader, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Erro no link do shader: ") + log);
    }

    // VBO: 4 vértices, cada um com 4 floats (posição XY + textura UV).
    // EBO: os índices reutilizam vértices para formar os dois triângulos.
    const float vertices[] = {
        // x    y    u    v
        0.f, 0.f, 0.f, 0.f, // canto superior esquerdo
        1.f, 0.f, 1.f, 0.f, // canto superior direito
        1.f, 1.f, 1.f, 1.f, // canto inferior direito
        0.f, 1.f, 0.f, 1.f  // canto inferior esquerdo
    };
    const unsigned indices[] = {0, 1, 2, 2, 3, 0};
    glGenVertexArrays(1, &vao); glGenBuffers(1, &vbo); glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glUseProgram(shader);
    uModel = glGetUniformLocation(shader, "model");
    uProjection = glGetUniformLocation(shader, "projection");
    uUV = glGetUniformLocation(shader, "uvRect");
    uTint = glGetUniformLocation(shader, "tint");
    uRadius = glGetUniformLocation(shader, "cornerRadius");
    uRectSize = glGetUniformLocation(shader, "rectSize");
    glUniform1i(glGetUniformLocation(shader, "imageTexture"), 0);
    // left=0, right=1280, bottom=720, top=0: a origem fica no topo.
    const glm::mat4 projection = glm::ortho(0.f, WORLD_W, WORLD_H, 0.f, -1.f, 1.f);
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projection));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); // A ordem dos desenhos determina as camadas.

    // Textura branca de 1 pixel permite desenhar também painéis com o quad.
    glGenTextures(1, &white); glBindTexture(GL_TEXTURE_2D, white);
    const unsigned char pixel[] = {255,255,255,255};
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,pixel);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    textures["dog"] = loadTexture(root / config.dogTexture);
    textures["sky"] = loadTexture(root / config.skyTexture);
    textures["hills"] = loadTexture(root / config.hillsTexture);
    textures["trees"] = loadTexture(root / config.treesTexture);
    textures["ground"] = loadTexture(root / config.groundTexture);
    textures["items"] = loadTexture(root / config.itemsTexture);
    textures["font"] = loadTexture(root / "assets/font.png");
    std::ifstream metrics(root / "assets/font.metrics");
    for (float& advance : advances)
        if (!(metrics >> advance)) throw std::runtime_error("Atlas de fonte: metricas invalidas.");
}

Renderer::~Renderer() {
    for (const auto& entry : textures) glDeleteTextures(1, &entry.second);
    glDeleteTextures(1, &white);
    glDeleteBuffers(1, &ebo); glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao); glDeleteProgram(shader);
}

GLuint Renderer::loadTexture(const std::filesystem::path& filename) {
    int width, height, channels;
    // Mantemos a primeira linha do PNG em v=0. O quad usa v=0 no topo,
    // portanto não é necessário inverter a imagem ao carregar.
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(filename.string().c_str(), &width, &height, &channels, 4);
    if (!data) throw std::runtime_error("Falha ao abrir imagem: " + filename.string());
    GLuint texture; glGenTextures(1, &texture); glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    // Sem mipmaps: atlas 2D usa somente nível 0 e não mistura células distantes.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return texture;
}

void Renderer::draw(GLuint texture, Rect rect, glm::vec4 uv, glm::vec4 tint,
                    float angle, float radius) {
    // O quad unitário é centralizado, escalado, rotacionado e transladado.
    // A matriz é aplicada da direita para a esquerda no vertex shader.
    glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(rect.x+rect.w/2,rect.y+rect.h/2,0));
    model = glm::rotate(model, angle, glm::vec3(0,0,1));
    model = glm::scale(model, glm::vec3(rect.w,rect.h,1));
    model = glm::translate(model, glm::vec3(-.5f,-.5f,0));
    glUniformMatrix4fv(uModel,1,GL_FALSE,glm::value_ptr(model));
    glUniform4fv(uUV,1,glm::value_ptr(uv));
    glUniform4fv(uTint,1,glm::value_ptr(tint));
    glUniform1f(uRadius,radius);
    glUniform2f(uRectSize,rect.w,rect.h);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
}
void Renderer::box(Rect rect, glm::vec4 color, float radius) { draw(white,rect,{0,0,1,1},color,0,radius); }
void Renderer::outline(Rect r, glm::vec4 c) {
    box({r.x,r.y,r.w,2},c); box({r.x,r.y+r.h-2,r.w,2},c);
    box({r.x,r.y,2,r.h},c); box({r.x+r.w-2,r.y,2,r.h},c);
}
void Renderer::panel(Rect r) {
    box({r.x,r.y+6,r.w,r.h},{.11f,.24f,.20f,.16f},22);
    box(r,{.35f,.49f,.34f,.6f},22);
    box({r.x+2,r.y+2,r.w-4,r.h-4},PAPER,20);
}
void Renderer::text(const std::string& value, float x, float y, float size, glm::vec4 color) {
    const float scale = size / 84.f;
    for (const unsigned code : letters(value)) {
        if (code != ' ')
            draw(textures.at("font"), {x-4*scale,y-16*scale,128*scale,128*scale},
                 {(code%16)/16.f+.5f/2048,(code/16)/16.f+.5f/2048,
                  1.f/16-1.f/2048,1.f/16-1.f/2048},color);
        x += advances[code] * scale;
    }
}
float Renderer::textWidth(const std::string& value, float size) const {
    float width=0; for (unsigned code : letters(value)) width += advances[code];
    return width * size / 84.f;
}
void Renderer::centered(const std::string& s,float centerX,float y,float size,glm::vec4 color) {
    text(s,centerX-textWidth(s,size)/2,y,size,color);
}
void Renderer::item(int frame,Rect rect,glm::vec4 tint,float angle) {
    draw(textures.at("items"),rect,{(frame%4)*.25f,(frame/4)*.5f,.25f,.5f},tint,angle);
}

void Renderer::background(float scroll) {
    draw(textures.at("sky"),{0,0,WORLD_W,WORLD_H});
    // O cenário se desloca em velocidades proporcionais à distância corrida.
    // Duas cópias lado a lado evitam lacunas; fmod faz a repetição contínua.
    const auto layer = [&](const char* name,float factor) {
        const float x = -std::fmod(scroll * factor,WORLD_W);
        draw(textures.at(name),{x,0,WORLD_W,WORLD_H});
        draw(textures.at(name),{x+WORLD_W,0,WORLD_W,WORLD_H});
    };
    layer("hills",.13f); layer("trees",.40f); layer("ground",1.f);
}

void Renderer::hud(const Game& g) {
    panel({28,24,230,84});
    text("PONTOS",48,39,13,MUTED);
    std::ostringstream score; score << std::setw(5) << std::setfill('0') << g.score();
    text(score.str(),46,65,28,INK);
    item(0,{181,53,51,45});
    panel({275,24,172,84});
    text("PETISCOS",295,39,13,MUTED);
    text(std::to_string(g.collected),295,65,28,INK);
    panel({982,24,270,84});
    text("VIDAS",1002,39,13,MUTED);
    for (int i=0;i<g.config.lives;++i)
        item(7,{1000.f+i*44,60,38,35},i<g.lives?glm::vec4(1):glm::vec4(.5f,.6f,.5f,.30f));
    centered("UM PETISCO DE CADA VEZ",704,42,14,MUTED);
    centered(std::to_string(static_cast<int>(g.distance/50)) + " m  /  PARQUE",704,69,16,INK);

    box({28,629,1224,62},{.99f,.97f,.89f,.94f},18);
    text("ESPAÇO  PULAR",52,650,16,INK);
    text("A / D  MOVER",330,650,16,MUTED);
    text("P  PAUSAR",582,650,16,MUTED);
    text("H  COLISÕES",804,650,16,MUTED);
    text("ESC  MENU",1053,650,16,MUTED);
    if (g.debugHitboxes) {
        box({28,122,293,35},{.1f,.23f,.20f,.90f},8);
        text("HITBOXES AABB ATIVADAS",40,133,13,PAPER);
    } else if (g.time < 4.f) {
        centered("Pegue os petiscos e pule os obstáculos!",640,148,22,INK);
    }
}

void Renderer::menu(float t) {
    panel({60,118,666,419});
    box({92,150,273,31},{.88f,.91f,.78f,1},10);
    text("UM PASSEIO, MUITAS AVENTURAS",103,160,12,MUTED);
    text("PATINHAS",92,206,64,INK);
    text("EM MISSÃO",92,279,60,TEAL);
    text("Corra pelo parque. Colete petiscos.",96,360,20,MUTED);
    text("E não esqueça de pular!",96,392,20,MUTED);
    box({94,449,273,56},TEAL,16);
    centered("ENTER  COMEÇAR",230.5f,467,19,PAPER);
    text("ESPAÇO para pular",399,470,16,MUTED);

    // A pose feliz ocupa a última célula da spritesheet.
    float bounce=std::sin(t*2)*3;
    draw(textures.at("dog"),{826,270+bounce,324,324},{.75f,.5f,.25f,.5f});
    item(static_cast<int>(t*6)%4,{1010,210+std::sin(t*3)*6,60,60},{1,1,1,1},-.15f);
    box({891,558,198,33},{.99f,.97f,.89f,.88f},12);
    centered("SEU PARCEIRO DE MISSÃO",990,569,11,INK);
    centered("3 VIDAS   /   +10 POR PETISCO   /   DIVERSÃO SEM FIM",640,659,15,INK);
}

void Renderer::render(const Game& g, GLFWwindow* window, float menuTime) {
    int width,height; glfwGetFramebufferSize(window,&width,&height);
    if (width<=0 || height<=0) return;
    // Letterbox: preserva o aspecto 16:9, inclusive ao redimensionar a janela.
    const float scale=std::min(width/WORLD_W,height/WORLD_H);
    const int viewW=static_cast<int>(WORLD_W*scale), viewH=static_cast<int>(WORLD_H*scale);
    glViewport(0,0,width,height); glClearColor(.08f,.15f,.13f,1); glClear(GL_COLOR_BUFFER_BIT);
    glViewport((width-viewW)/2,(height-viewH)/2,viewW,viewH);
    glUseProgram(shader);
    background(g.screen==Screen::Menu ? menuTime*36 : g.distance);
    if (g.screen==Screen::Menu) { menu(menuTime); return; }

    for (const auto& t : g.treats) {
        const float bob=std::sin(g.time*4+t.phase)*5;
        const int frame=static_cast<int>(g.time*7+t.phase)%4;
        item(frame,{t.x,t.y+bob,44,44});
        if (g.debugHitboxes) outline(t.bounds(g.time),{1,.75f,0,1});
    }
    for (const auto& o : g.obstacles) {
        const int frame=4+static_cast<int>(o.type);
        if (o.type==ObstacleType::Log) item(frame,{o.x,GROUND_Y-73,96,83});
        else if(o.type==ObstacleType::Rock) item(frame,{o.x,GROUND_Y-68,76,80});
        else item(frame,{o.x,GROUND_Y-34,126,42});
        if (g.debugHitboxes) outline(o.bounds(),{.95f,.25f,.20f,1});
    }
    const int frame=g.dogFrame();
    float alpha=(g.player.immuneTime>0 && static_cast<int>(g.player.immuneTime*12)%2==0) ? .40f : 1.f;
    draw(textures.at("dog"),{g.player.x,GROUND_Y-DOG_SIZE*.83f-g.player.height,DOG_SIZE,DOG_SIZE},
         {(frame%4)*.25f,(frame/4)*.5f,.25f,.5f},{1,1,1,alpha});
    if (g.debugHitboxes) outline(g.player.bounds(),{.05f,.7f,.8f,1});
    for (const auto& p : g.particles) {
        glm::vec4 color=GOLD; color.a=p.life/p.maxLife;
        box({p.x,p.y,6,6},color,2);
    }
    if(g.hitFlash>0 && g.screen==Screen::Playing)
        box({0,0,WORLD_W,WORLD_H},{.85f,.29f,.17f,g.hitFlash*.35f});
    hud(g);

    if(g.screen==Screen::Paused || g.screen==Screen::GameOver) {
        box({0,0,WORLD_W,WORLD_H},{.08f,.18f,.15f,.46f});
        panel({367,175,546,360});
        const bool paused=g.screen==Screen::Paused;
        centered(paused?"PAUSA PARA UM CARINHO":"FIM DO PASSEIO",640,218,paused?24:32,INK);
        centered(paused?"Seu parceiro espera por você.":"Cada passeio é uma nova aventura.",640,278,18,MUTED);
        if (!paused) {
            centered(std::to_string(g.score())+" PONTOS",640,328,36,TEAL);
            centered("RECORDE DA SESSÃO: "+std::to_string(g.bestScore),640,381,14,MUTED);
        } else {
            item(7,{609,327,62,58});
        }
        box({432,429,416,57},TEAL,15);
        centered(paused?"P / ENTER  CONTINUAR":"ENTER / R  JOGAR NOVAMENTE",640,448,17,PAPER);
        centered("ESC  VOLTAR AO MENU",640,506,12,MUTED);
    }
}

void Renderer::screenshot(const std::string& filename,GLFWwindow* window) {
    int w,h; glfwGetFramebufferSize(window,&w,&h);
    if(w<=0 || h<=0) return;
    std::vector<unsigned char> pixels(static_cast<size_t>(w)*h*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    GLboolean doubleBuffered=GL_FALSE;
    glGetBooleanv(GL_DOUBLEBUFFER,&doubleBuffered);
    glReadBuffer(doubleBuffered?GL_BACK:GL_FRONT);
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream out(filename,std::ios::binary);
    if(!out) throw std::runtime_error("Falha ao salvar captura: "+filename);
    out << "P6\n" << w << ' ' << h << "\n255\n";
    for(int y=h-1;y>=0;--y)
        out.write(reinterpret_cast<const char*>(pixels.data()+static_cast<size_t>(y)*w*3),w*3);
}
