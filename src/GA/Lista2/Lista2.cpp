// ============================================================================
// LISTA 2 - PROCESSAMENTO GRAFICO (OPENGL)
// ============================================================================
// Este programa reúne seis exercícios. Ao apertar de 1 a 6, trocamos o exemplo.
// Cada desenho representa um conjunto de pontos (vértices) ligados entre si.
// O C++ organiza os pontos; o OpenGL envia os dados à placa gráfica (GPU),
// calcula onde eles aparecem e colore a região correspondente na tela.

// Referencia: https://github.com/fellowsheep/PG2026-2
// GLAD disponibiliza as funcoes modernas de OpenGL.
#include <glad/glad.h>
// GLFW cria a janela e captura eventos de teclado e mouse.
#include <GLFW/glfw3.h>
// GLM oferece vetores, matrizes, projeção ortográfica e acesso aos dados da matriz.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <array>
// Cada vertice e uma "pontinha" da figura.
// x = horizontal; y = vertical; z = profundidade.
// Como os exemplos sao 2D, deixamos z igual a zero.
struct Vertex { float x, y, z; };
// No exercicio 6, um triangulo so fica pronto depois de tres cliques.
// Esta estrutura guarda as tres posicoes e uma cor para cada triangulo.
// Guardamos a posicao de cada uma das tres pontas do triangulo.
// No exercicio 6, cada ponta e escolhida por um clique do mouse.
struct TriangleInfo { std::array<Vertex, 3> vertices; glm::vec3 color; };
// Estas variaveis precisam existir fora do main porque sao usadas
// tanto pelo loop de desenho quanto pelas funcoes chamadas pelo mouse/teclado.
int exercise = 1; // Modo inicial; teclas 1 a 6 alteram esta variável.
bool dirty = true; // Há dados de mouse a atualizar no VBO?
// Vetor contíguo usado no upload: mantém inclusive os 1-2 vértices pendentes.
std::vector<Vertex> clicks;
std::vector<TriangleInfo> triangles;
// Ao completar tres cliques, reunimos os tres ultimos pontos em uma
// estrutura TriangleInfo. Ex.: indices 0,1,2 formam o primeiro triangulo;
// indices 3,4,5 formam o segundo. Os pontos continuam no vetor clicks.
// O vetor triangles guarda os dados dos triângulos concluídos; clicks guarda
// a sequência completa enviada ao VBO, na mesma ordem desses triângulos.
// Esta funcao so e chamada quando clicks ja tem pelo menos 3 pontos.
// Ela nao inventa vertices: aproveita exatamente os ultimos tres cliques.
void addTriangle() {
    std::size_t start = clicks.size() - 3;
    TriangleInfo tri;
    tri.vertices = {clicks[start], clicks[start + 1], clicks[start + 2]};
    tri.color = glm::vec3(1.0f); // Definida pelo callback ao completar o trio.
    triangles.push_back(tri);
}
// Para os triangulos nao ficarem todos da mesma cor, geramos uma nova
// cor a cada tres cliques. O matiz percorre o circulo de cores (HSV),
// e a funcao o converte para vermelho, verde e azul (RGB).
glm::vec3 nextColor(std::size_t index) {
    // O incremento de matiz distribui as cores pelo círculo cromático.
    float h = std::fmod(float(index) * 0.61803398875f, 1.0f) * 6.0f;
    float x = 1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f);
    glm::vec3 c;
    if (h < 1) c = {1,x,0}; else if (h < 2) c = {x,1,0};
    else if (h < 3) c = {0,1,x}; else if (h < 4) c = {0,x,1};
    else if (h < 5) c = {x,0,1}; else c = {1,0,x};
    return c * 0.75f + glm::vec3(0.2f);
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    // Se a tecla estiver sendo solta ou repetida, saimos imediatamente.
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GLFW_TRUE);
    // Uma tecla numerica escolhe qual exercicio sera mostrado no proximo frame.
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        // Os códigos das teclas numéricas são consecutivos: converte a tecla em 1-6.
        exercise = key - GLFW_KEY_0;
        std::string title = "Lista 2 - Exercicio " + std::to_string(exercise);
        glfwSetWindowTitle(window, title.c_str());
    }
    // C apaga os triangulos criados com o mouse. dirty avisa que o
    // VBO precisa receber a nova lista (agora vazia).
    if (key == GLFW_KEY_C) { clicks.clear(); triangles.clear(); dirty = true; }
}
// Quando clicamos com o botao esquerdo no exercicio 6, adicionamos
// um novo vertice. A funcao NAO desenha por conta propria: so muda
// os dados; o loop principal cuida de atualizar a tela.
// O callback não desenha: apenas atualiza os dados que o loop renderiza depois.
void mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    if (exercise != 6 || button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;
    int w, h;
    // Tamanho lógico: corresponde às unidades das coordenadas fornecidas pelo mouse.
    glfwGetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) return;
    double x, y;
    // Recebemos a posicao do mouse em coordenadas da janela:
    // (0,0) e o canto superior esquerdo, e y aumenta para baixo.
    glfwGetCursorPos(window, &x, &y);
    if (x < 0 || x >= w || y < 0 || y >= h) return;
    // Transformamos a posicao do mouse para o nosso mundo 800x600.
    clicks.push_back({float(x * 800.0 / w), float(600.0 - y * 600.0 / h), 0.0f});
    // Resto zero na divisão por 3 significa que um novo trio ficou completo.
    if (clicks.size() % 3 == 0) {
        addTriangle();
        triangles.back().color = nextColor(triangles.size() - 1);
    }
    dirty = true;
    std::cout << "Vertices: " << clicks.size() << " | Triangulos: " << triangles.size() << '\n';
}
// PREPARAÇÃO COMUM — compila um estágio de shader na GPU.
// Em falha, recupera o log do driver e interrompe a criação do programa.
GLuint compile(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader); throw std::runtime_error(log);
    }
    return shader;
}
// PREPARAÇÃO COMUM — vertex shader transforma posições; fragment shader define cor.
GLuint setupShader() {
    const char* vs = R"(#version 330 core
// location=0 corresponde ao atributo configurado no VAO.
layout(location=0) in vec3 position;
// Uniform: matriz compartilhada por todos os vértices de uma chamada de desenho.
uniform mat4 projection;
void main() { gl_Position = projection * vec4(position, 1.0); }
)";
    const char* fs = R"(#version 330 core
uniform vec4 inputColor;
out vec4 color;
void main() { color = inputColor; }
)";
    GLuint v = compile(GL_VERTEX_SHADER, vs), f = compile(GL_FRAGMENT_SHADER, fs);
    GLuint program = glCreateProgram();
    glAttachShader(program, v); glAttachShader(program, f); glLinkProgram(program);
    glDeleteShader(v); glDeleteShader(f);
    GLint ok; glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        glDeleteProgram(program); throw std::runtime_error(log);
    }
    return program;
}
// PREPARAÇÃO COMUM — configura a leitura de um VBO pelo VAO.
void setupBuffer(GLuint& vao, GLuint& vbo) {
    glGenVertexArrays(1, &vao); glGenBuffers(1, &vbo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);
}
// ============================================================================
// FUNCAO MAIN:
// ============================================================================
int main()
{
    // 1. Inicia a GLFW, responsável pela janela, contexto e eventos de entrada.
    if (!glfwInit()) {
        std::cerr << "Falha ao iniciar GLFW\n";
        return 1;
    }
    // 2. Solicita um contexto OpenGL 3.3 no perfil core (pipeline com shaders).
    // A versão GLSL 330 dos shaders corresponde a essa versão da OpenGL.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    // Configuração necessária para os contextos core no macOS.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // 3. Cria uma janela inicialmente com 800 x 600 unidades de tela.
    GLFWwindow* window = glfwCreateWindow(
        800, 600, "Lista 2 - Exercicio 1", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    // 4. GLAD carrega os endereços das funções OpenGL oferecidas pelo driver.
    // glfwGetProcAddress fornece esses endereços para o contexto recém-criado.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    // Sincroniza a apresentação de quadros com a atualização do monitor (VSync).
    glfwSwapInterval(1);
    // 5. Registra callbacks. A GLFW chama essas funções quando processa eventos.
    // Teclado: seleciona exercícios, limpa os cliques ou encerra a aplicação.
    // Mouse: acrescenta um vértice por pressionamento, somente no exercício 6.
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // 6. Compila os shaders e faz a linkagem do programa que será usado na GPU.
    GLuint program;
    try {
        program = setupShader();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    // 7. Separa os buffers da cena fixa dos buffers de entrada do mouse.
    // VAO: guarda a configuração de leitura dos atributos dos vértices.
    // VBO: armazena as posições dos vértices na memória acessível à GPU.
    GLuint sceneVAO, sceneVBO, mouseVAO, mouseVBO;
    setupBuffer(sceneVAO, sceneVBO); 
    setupBuffer(mouseVAO, mouseVBO); 
    // Seleciona o programa ativo. As próximas uniforms serão enviadas a ele.
    glUseProgram(program);
    GLint projectionLoc = glGetUniformLocation(program, "projection");
    GLint colorLoc = glGetUniformLocation(program, "inputColor");
    std::cout << "Teclas 1-6: exercicios | C: limpar | ESC: sair\n";
    
    // EXERCICIO 1: tres pontos formam um triangulo num mundo de -10 a 10.
    // A base esta em y=-4 e a ponta em y=4; o centro e (0,0).
    const Vertex centered[] = {
        {-4.0f, -4.0f, 0.0f}, // Canto inferior esquerdo.
        { 4.0f, -4.0f, 0.0f}, // Canto inferior direito.
        { 0.0f,  4.0f, 0.0f}  // Ponta superior.
    };
    // EXERCICIOS 2, 4 E 5: mesmo triangulo, mas agora as posicoes
    // sao descritas em um mundo de 800 por 600 unidades.
    // Como o eixo y cresce para BAIXO nesta projecao, y=150 fica acima de y=450.
    // Nesse mundo y cresce para baixo; a ponta tem y menor que o da base.
    const Vertex pixels[] = {
        {200.0f, 450.0f, 0.0f}, // Base esquerda.
        {600.0f, 450.0f, 0.0f}, // Base direita.
        {400.0f, 150.0f, 0.0f}  // Ponta superior.
    };
    // EXERCICIO 3: um quadrado e formado por DOIS triangulos.
    // Dois triangulos juntos formam um quadrado (6 vertices no total).
    const Vertex square[] = {
        {200.0f, 200.0f, 0.0f}, // Superior esquerdo.
        {400.0f, 200.0f, 0.0f}, // Superior direito.
        {400.0f, 400.0f, 0.0f}, // Inferior direito.
        {200.0f, 200.0f, 0.0f}, // Superior esquerdo novamente.
        {400.0f, 400.0f, 0.0f}, // Inferior direito novamente.
        {200.0f, 400.0f, 0.0f}  // Inferior esquerdo.
    };
    // Esta variável evita reenviar a geometria fixa em todos os quadros.
    int previousExercise = 0;
    // ========================================================================
    // LOOP DA APLICAÇÃO:
    // ========================================================================
    // GAME LOOP: enquanto a janela estiver aberta, repetimos a sequencia
    // ler eventos -> atualizar o que mudou -> desenhar -> apresentar o frame.
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w <= 0 || h <= 0) {
            glfwWaitEvents();
            continue;
        }
        // Define o fundo em RGBA e limpa o buffer de cor uma vez por quadro.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // Apagamos o desenho do frame anterior antes de preparar o novo.
        // Se nao limpassemos, as imagens poderiam ficar acumuladas.
        glClear(GL_COLOR_BUFFER_BIT);
        // Preparação compartilhada dos exercícios 1 a 5.
        if (exercise != 6) {
            if (previousExercise != exercise) {
                // Selecionamos o VBO dos exercicios fixos (1 a 5).
                // O conteudo so precisa ser reenviado quando trocamos de exercicio.
                glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
                if (exercise == 3) {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
                } else {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(pixels),
                        exercise == 1 ? centered : pixels, GL_STATIC_DRAW);
                }
            }
            // Escolhemos o VAO que sabe como ler os vertices da cena fixa.
            glBindVertexArray(sceneVAO);
            // Cor magenta uniforme para o triângulo fixo, como na figura da lista.
            glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f);
        }
        glm::mat4 projection(1.0f);
        int halfW = w / 2;
        int halfH = h / 2;
        
        // Cada case abaixo resolve um exercício completo e está identificado.
        // O switch verifica o numero escolhido pelo teclado e executa
        // SOMENTE o bloco de desenho do exercicio correspondente.
        switch (exercise)
        {
            // ================================================================
            // EXERCÍCIO 1 — xmin=-10, xmax=10, ymin=-10, ymax=10
            // ================================================================
            // Sistema centrado em (0,0), indo de -10 ate +10.
            // Desenhamos um triangulo e a projecao o ajusta ao viewport.
            case 1:
            {
                // Origem central; x cresce para a direita, y cresce para cima.
                // near=-1 e far=1 incluem os vértices desenhados em z=0.
                projection = glm::ortho(
                    -10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
                // Envia 1 matriz 4x4. GL_FALSE: sem transposição.
                // value_ptr fornece o endereço dos valores armazenados pela GLM.
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glViewport(0, 0, w, h); // Ocupa todo o framebuffer.
                // GL_TRIANGLES agrupa cada 3 vértices; começa em 0 e lê 3.
                glDrawArrays(GL_TRIANGLES, 0, 3);
                break;
            }
            // ================================================================
            // EXERCÍCIO 2 — xmin=0, xmax=800, ymin=600, ymax=0
            // ================================================================
            // Origem no alto a esquerda; 
            // O triangulo usa posicoes semelhantes a unidades de tela.
            case 2:
            {
                // bottom=600 e top=0 invertem o sentido visual do eixo y.
                projection = glm::ortho(
                    0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glViewport(0, 0, w, h);
                // Exibe o triângulo em pixels para visualizar a nova câmera.
                glDrawArrays(GL_TRIANGLES, 0, 3);
                break;
            }
            // ================================================================
            // EXERCÍCIO 3 
            // ================================================================
            // Usamos o mesmo sistema do 2, mas desenhamos um quadrado
            // com dois triangulos. 
            case 3:
            {
                // Mantém exatamente a câmera do exercício anterior.
                projection = glm::ortho(
                    0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glViewport(0, 0, w, h);
                glUniform4f(colorLoc, 1.0f, 0.85f, 0.0f, 1.0f);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // RESPOSTA: aumentar x move para a direita; aumentar y move para baixo.
                // Isso facilita posicionar interfaces e sprites com coordenadas de tela.
                // Com viewport 800x600, uma unidade equivale a um pixel do framebuffer.
                // Essa camera facilita posicionar interfaces e elementos de jogos.

                break;
            }
            // ================================================================
            // EXERCÍCIO 4 — desenhar somente no quadrante SUPERIOR DIREITO
            // ================================================================
            // Limitamos o desenho ao quadrante
            // superior direito do framebuffer. Viewport muda ONDE a imagem aparece.
            case 4:
            {
                projection = glm::ortho(
                    0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glViewport(halfW, halfH, w - halfW, h - halfH);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                break;
            }
            // ================================================================
            // EXERCÍCIO 5 
            // ================================================================
            // Repetimos o mesmo desenho quatro vezes, em quatro
            // viewports. Nao mudamos os vertices; apenas a regiao onde aparecem.
            case 5:
            {
                projection = glm::ortho(
                    0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                // Inferior esquerdo: para 800x600, (0,0,400,300).
                glViewport(0, 0, halfW, halfH);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                // Inferior direito: (400,0,400,300).
                glViewport(halfW, 0, w - halfW, halfH);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                // Superior esquerdo: (0,300,400,300).
                glViewport(0, halfH, halfW, h - halfH);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                // Superior direito: (400,300,400,300).
                glViewport(halfW, halfH, w - halfW, h - halfH);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                // Reutilizamos o mesmo VAO/VBO: somente o viewport muda.
                // w-halfW e h-halfH incluem o pixel restante em tamanhos ímpares.
                break;
            }
            // ================================================================
            // EXERCÍCIO 6 — um vértice por clique; um triângulo a cada 3 vértices.
            // ================================================================
            case 6:
            {
                projection = glm::ortho(
                    0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
                glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glViewport(0, 0, w, h);
                glBindVertexArray(mouseVAO);
                // dirty indica que um clique ou a tecla C alterou o vetor.
                // Assim não reenviamos os mesmos dados à GPU a cada quadro.
                if (dirty) {
                    glBindBuffer(GL_ARRAY_BUFFER, mouseVBO);
                    glBufferData(GL_ARRAY_BUFFER, clicks.size() * sizeof(Vertex),
                        clicks.empty() ? nullptr : clicks.data(), GL_DYNAMIC_DRAW);
                    dirty = false;
                }
                for (std::size_t i = 0; i < triangles.size(); ++i) {
                    const auto& c = triangles[i].color;
                    glUniform4f(colorLoc, c.r, c.g, c.b, 1.0f);
                    glDrawArrays(GL_TRIANGLES, GLint(i * 3), 3);
                }
                // Um ou dois cliques restantes ainda não formam um triângulo.
                // Mostra esses vértices como pontos brancos para orientar o usuário.
                GLint complete = GLint(triangles.size() * 3);
                glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
                glPointSize(7.0f);
                glDrawArrays(GL_POINTS, complete, GLsizei(clicks.size() - complete));
                break;
            }
        }
        // Registra o modo desenhado para saber se a geometria fixa precisa mudar.
        previousExercise = exercise;
        // Apresenta o quadro concluído trocando os buffers frontal e traseiro.
        // Troca o quadro que estava sendo preparado pelo quadro visivel.
        // Assim, o usuario ve um frame completo em vez do desenho pela metade.
        glfwSwapBuffers(window);
    }
    // ========================================================================
    // Quando fechamos a janela, apagamos os objetos que criamos na GPU
    // e encerramos a GLFW. Isso evita deixar recursos alocados sem necessidade.
    // ========================================================================
    glDeleteVertexArrays(1, &sceneVAO);
    glDeleteVertexArrays(1, &mouseVAO);
    glDeleteBuffers(1, &sceneVBO);
    glDeleteBuffers(1, &mouseVBO);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
