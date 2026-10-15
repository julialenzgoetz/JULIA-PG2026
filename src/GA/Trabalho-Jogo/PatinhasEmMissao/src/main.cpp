/*
 * PATINHAS EM MISSÃO — Processamento Gráfico: Fundamentos
 * C++17 / OpenGL 3.3 Core / GLFW / GLAD / GLM / stb_image
 *
 * Comece a leitura por este arquivo. Ele organiza a aplicação inteira:
 *   1. Encontrar os recursos e ler opções de execução.
 *   2. Inicializar GLFW e criar a janela/contexto OpenGL.
 *   3. Carregar as funções OpenGL usando GLAD.
 *   4. Criar Game (regras) e Renderer (desenho).
 *   5. Processar teclado sem repetir ações de um toque.
 *   6. Atualizar a física com passo fixo.
 *   7. Renderizar, apresentar o frame e liberar os recursos.
 *
 * Não há glBegin/glEnd nem pipeline fixo: todo desenho usa shaders,
 * VAO, VBO e EBO. O código gráfico está explicado em Renderer.cpp.
 */
#include "Game.hpp"
#include "Renderer.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

// 1. Os recursos podem estar junto ao executável, na pasta atual ou na raiz
// do projeto. Isso permite abrir pelo terminal, script ou IDE.
fs::path findAssets(const char* executable, const std::string& explicitRoot) {
    if (!explicitRoot.empty()) return fs::absolute(explicitRoot);
    fs::path exeDir = fs::absolute(executable).parent_path();
    const fs::path candidates[] = {fs::current_path(),exeDir,exeDir.parent_path(),
        exeDir.parent_path().parent_path(),fs::path(PATINHAS_SOURCE_DIR)};
    for (const auto& root : candidates)
        if (fs::exists(root/"config/game.cfg") && fs::exists(root/"shaders/sprite.vert"))
            return root;
    throw std::runtime_error("Pasta de recursos nao encontrada. Execute na raiz do projeto ou use --assets PASTA.");
}

// PRESS representa um toque. Ignoramos REPEAT para evitar que segurar espaço
// dispare um novo pulo assim que o cachorro volte ao chão.
void onKey(GLFWwindow* window, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!game) return;
    if (key == GLFW_KEY_H) { game->debugHitboxes = !game->debugHitboxes; return; }
    if (key == GLFW_KEY_ESCAPE) {
        if (game->screen == Screen::Menu) glfwSetWindowShouldClose(window,true);
        else game->screen = Screen::Menu;
        return;
    }
    if (key == GLFW_KEY_ENTER) {
        if (game->screen == Screen::Menu || game->screen == Screen::GameOver) game->start();
        else if (game->screen == Screen::Paused) game->togglePause();
    } else if (key == GLFW_KEY_P) game->togglePause();
    else if (key == GLFW_KEY_R && game->screen != Screen::Menu) game->start();
    else if (key == GLFW_KEY_SPACE || key == GLFW_KEY_UP || key == GLFW_KEY_W) game->jump();
}

void onFocus(GLFWwindow* window,int focused) {
    auto* game=static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (!focused && game && game->screen==Screen::Playing) game->togglePause();
}

int main(int argc,char** argv) {
    GLFWwindow* window=nullptr;
    try {
        // Estas opções são auxiliares de teste; para jogar basta abrir o programa.
        bool headless=false,smoke=false,autoJump=false,debug=false;
        int frameLimit=0;
        std::string capture,rootArgument,scene="menu";
        for(int i=1;i<argc;++i) {
            std::string arg=argv[i];
            auto next=[&]() -> std::string {
                if(i+1>=argc) throw std::runtime_error("Falta valor para "+arg);
                return argv[++i];
            };
            if(arg=="--headless") headless=true;
            else if(arg=="--smoke-test") {smoke=true;autoJump=true;frameLimit=1200;scene="play";}
            else if(arg=="--frames") frameLimit=std::stoi(next());
            else if(arg=="--capture") capture=next();
            else if(arg=="--assets") rootArgument=next();
            else if(arg=="--scene") scene=next();
            else if(arg=="--auto-jump") autoJump=true;
            else if(arg=="--hitboxes") debug=true;
            else if(arg=="--help") {
                std::cout << "PatinhasEmMissao [--assets PASTA] [--scene menu|play|paused|gameover]\n"
                             "  [--frames N] [--capture imagem.ppm] [--smoke-test] [--headless]\n";
                return 0;
            } else throw std::runtime_error("Opcao desconhecida: "+arg);
        }
        if(frameLimit<0) throw std::runtime_error("--frames precisa ser positivo.");
        if(scene!="menu" && scene!="play" && scene!="paused" && scene!="gameover")
            throw std::runtime_error("Cena invalida: "+scene);
        const fs::path root=findAssets(argv[0],rootArgument);
        const Config config=loadConfig((root/"config/game.cfg").string());

        // 2. GLFW cuida da janela, do contexto e dos eventos de entrada.
        glfwSetErrorCallback([](int code,const char* description) {
            std::cerr << "GLFW " << code << ": " << description << '\n';
        });
        if(headless) glfwInitHint(GLFW_PLATFORM,GLFW_PLATFORM_NULL);
        if(!glfwInit()) throw std::runtime_error("Nao foi possivel inicializar GLFW.");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
        if(headless) {
            glfwWindowHint(GLFW_CONTEXT_CREATION_API,GLFW_OSMESA_CONTEXT_API);
            glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        }
        window=glfwCreateWindow(config.windowWidth,config.windowHeight,"Patinhas em Missao",nullptr,nullptr);
        if(!window) throw std::runtime_error("Falha ao criar contexto OpenGL 3.3. Verifique o driver de video.");
        glfwMakeContextCurrent(window);
        glfwSwapInterval(smoke||frameLimit>0?0:1); // VSync durante o jogo normal.
        if(!headless) glfwSetWindowSizeLimits(window,640,360,GLFW_DONT_CARE,GLFW_DONT_CARE);

        // 3. GLAD busca no driver os endereços das funções de OpenGL moderno.
        if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            throw std::runtime_error("Falha ao carregar funcoes OpenGL.");
        std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
        std::cout << "GPU: " << glGetString(GL_RENDERER) << '\n';

        // 4. O bloco garante que Renderer libere texturas/buffers ANTES de
        // destruir a janela, enquanto o contexto OpenGL ainda é válido.
        {
            Game game(config,smoke?42u:std::random_device{}());
            Renderer renderer(root,config);
            glfwSetWindowUserPointer(window,&game);
            glfwSetKeyCallback(window,onKey);
            glfwSetWindowFocusCallback(window,onFocus);
            game.debugHitboxes=debug;
            if(scene!="menu") game.start();

            double previous=glfwGetTime(),accumulator=0;
            constexpr double STEP=1.0/120.0;
            int frames=0;
            float menuTime=0;
            while(!glfwWindowShouldClose(window)) {
                // 5. Eventos discretos (pulo, pausa) chegam ao callback.
                // A/D e setas são contínuos: consultamos a tecla pressionada.
                glfwPollEvents();
                float horizontal=0;
                if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS || glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS) horizontal-=1;
                if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS || glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) horizontal+=1;

                // 6. A física avança 1/120 s por passo, independentemente do FPS.
                // Limitamos o tempo de um frame a 0,1 s para não saltar através
                // de obstáculos ao retomar de uma interrupção longa.
                const double now=glfwGetTime();
                const double elapsed=frameLimit>0?STEP:std::min(now-previous,.10);
                previous=now;
                menuTime+=static_cast<float>(elapsed);
                accumulator+=elapsed;
                while(accumulator>=STEP) {
                    if(autoJump && game.screen==Screen::Playing && game.player.grounded) {
                        // Jogador automático apenas para teste de renderização.
                        for(const auto& obstacle:game.obstacles) {
                            const float gap=obstacle.bounds().x-(game.player.bounds().x+game.player.bounds().w);
                            if(gap>0 && gap<game.speed*.23f) {game.jump();break;}
                        }
                    }
                    game.update(static_cast<float>(STEP),horizontal);
                    accumulator-=STEP;
                }

                // 7. Limpa -> desenha cenário, sprites e interface -> troca buffers.
                // As cenas abaixo são usadas para capturas, não no jogo normal.
                if(frameLimit>0 && frames==frameLimit-1) {
                    if(scene=="paused") game.screen=Screen::Paused;
                    if(scene=="gameover") game.screen=Screen::GameOver;
                }
                renderer.render(game,window,menuTime);
                if(frameLimit>0 && frames==frameLimit-1 && !capture.empty())
                    renderer.screenshot(capture,window);
                const GLenum error=glGetError();
                if(error!=GL_NO_ERROR) throw std::runtime_error("Erro OpenGL: "+std::to_string(error));
                glfwSwapBuffers(window);
                ++frames;
                if(frameLimit>0 && frames>=frameLimit) break;
            }
            std::cout << "Frames: " << frames << " | pontos: " << game.score()
                      << " | petiscos: " << game.collected << " | vidas: " << game.lives << '\n';
            glfwSetWindowUserPointer(window,nullptr);
        }
        glfwDestroyWindow(window); glfwTerminate();
        return 0;
    } catch(const std::exception& error) {
        std::cerr << "ERRO: " << error.what() << '\n';
        if(window) glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
}
