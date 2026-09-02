#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

const GLuint WIDTH = 800;
const GLuint HEIGHT = 800;
const float PI = 3.14159265359f;


// =====================================================
// SHADERS
// =====================================================

const GLchar *vertexShaderSource = R"glsl(
#version 330 core

layout (location = 0) in vec3 position;

void main()
{
    gl_Position = vec4(position, 1.0);
}
)glsl";


const GLchar *fragmentShaderSource = R"glsl(
#version 330 core

uniform vec4 inputColor;

out vec4 color;

void main()
{
    color = inputColor;
}
)glsl";


// =====================================================
// PROTÓTIPOS
// =====================================================

void key_callback(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mode
);

GLuint setupShader();

GLuint criaVAO(
    GLfloat *vertices,
    unsigned int tamanho
);

vector<GLfloat> criaVerticesCirculo(
    float centroX,
    float centroY,
    float raio,
    int segmentos
);


// =====================================================
// MAIN
// =====================================================

int main()
{
    glfwInit();

    GLFWwindow *window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Exercicio 4 - Casinha",
        nullptr,
        nullptr
    );

    if (!window)
    {
        cerr << "Falha ao criar a janela GLFW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cerr << "Falha ao inicializar GLAD" << endl;
        return -1;
    }

    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    // =================================================
    // SHADER
    // =================================================

    GLuint shaderID = setupShader(); //compila Vertex Shader + Fragment Shader
    glUseProgram(shaderID);

    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");


    // =================================================
    // VÉRTICES
    // =================================================

    // -------------------------------------------------
    // CORPO DA CASA
    // -------------------------------------------------

    GLfloat corpo[] = //dois triângulos para a criação de um retângulo.
    {
        -0.5f, -0.6f, 0.0f,
         0.5f, -0.6f, 0.0f,
         0.5f,  0.2f, 0.0f, //triângulo 1

        -0.5f, -0.6f, 0.0f,
         0.5f,  0.2f, 0.0f,
        -0.5f,  0.2f, 0.0f //triângulo 2
    };


    // -------------------------------------------------
    // TELHADO
    // -------------------------------------------------

    GLfloat telhado[] = //triângulo
    {
        -0.6f, 0.2f, 0.0f,
         0.0f, 0.7f, 0.0f,
         0.6f, 0.2f, 0.0f
    };


    // -------------------------------------------------
    // PORTA
    // -------------------------------------------------

    GLfloat porta[] = //novamente, dois triângulos.
    {
        -0.15f, -0.6f,  0.0f,
         0.15f, -0.6f,  0.0f,
         0.15f, -0.15f, 0.0f,

        -0.15f, -0.6f,  0.0f,
         0.15f, -0.15f, 0.0f,
        -0.15f, -0.15f, 0.0f
    };


    // -------------------------------------------------
    // JANELA AZUL
    // -------------------------------------------------

    GLfloat janela[] =  //novamente, dois triângulos.
    {
        0.22f, -0.05f, 0.0f,
        0.42f, -0.05f, 0.0f,
        0.42f,  0.15f, 0.0f,

        0.22f, -0.05f, 0.0f,
        0.42f,  0.15f, 0.0f,
        0.22f,  0.15f, 0.0f
    };


    // -------------------------------------------------
    // CONTORNO DA JANELA
    // -------------------------------------------------

    GLfloat contornoJanela[] =
    {
        0.22f, -0.05f, 0.0f,
        0.42f, -0.05f, 0.0f,
        0.42f,  0.15f, 0.0f,
        0.22f,  0.15f, 0.0f  //quatro pontos conectados em linha.
    };


    // -------------------------------------------------
    // GRADE DA JANELA
    // -------------------------------------------------

    GLfloat gradeJanela[] = //duas linhas
    {
        0.32f, -0.05f, 0.0f,
        0.32f,  0.15f, 0.0f, //linha vertical

        0.22f,  0.05f, 0.0f,
        0.42f,  0.05f, 0.0f //linha horizontal
    };


    // -------------------------------------------------
    // CHÃO
    // -------------------------------------------------

    GLfloat chao[] = //uma única linha
    {
        -0.9f, -0.6f, 0.0f,
         0.9f, -0.6f, 0.0f
    };


    // -------------------------------------------------
    // MAÇANETA (CÍRCULO)
    // -------------------------------------------------

    vector<GLfloat> macaneta = criaVerticesCirculo(
        0.10f,   // centro X
       -0.38f,   // centro Y
        0.025f,  // raio
        32       // segmentos
    );


    // =================================================
    // CRIAÇÃO DOS VAOs
    // =================================================

    GLuint VAOcorpo = criaVAO(corpo, sizeof(corpo));
    GLuint VAOtelhado = criaVAO(telhado, sizeof(telhado));
    GLuint VAOporta = criaVAO(porta, sizeof(porta));
    GLuint VAOjanela = criaVAO(janela, sizeof(janela));
    GLuint VAOcontornoJanela = criaVAO(contornoJanela, sizeof(contornoJanela));
    GLuint VAOgradeJanela = criaVAO(gradeJanela, sizeof(gradeJanela));
    GLuint VAOchao = criaVAO(chao, sizeof(chao));
    GLuint VAOmacaneta = criaVAO(macaneta.data(), macaneta.size() * sizeof(GLfloat));


    // =================================================
    // LOOP DE RENDERIZAÇÃO
    // =================================================

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();


        // Fundo azul claro
        glClearColor(0.55f, 0.80f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderID);


        // =================================================
        // CORPO DA CASA
        // =================================================

        glBindVertexArray(VAOcorpo);
        glUniform4f(colorLoc, 0.95f, 0.65f, 0.25f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // =================================================
        // TELHADO
        // =================================================

        glBindVertexArray(VAOtelhado);
        glUniform4f(colorLoc, 0.85f, 0.05f, 0.02f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        // =================================================
        // PORTA
        // =================================================

        glBindVertexArray(VAOporta);
        glUniform4f(colorLoc, 0.40f, 0.15f, 0.03f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // =================================================
        // MAÇANETA
        // =================================================

        glBindVertexArray(VAOmacaneta);
        glUniform4f(colorLoc, 0.95f, 0.85f, 0.20f, 1.0f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, macaneta.size() / 3);


        // =================================================
        // JANELA
        // =================================================

        glBindVertexArray(VAOjanela);
        glUniform4f(colorLoc, 0.05f, 0.20f, 0.85f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // =================================================
        // CONTORNO DA JANELA
        // =================================================

        glLineWidth(4.0f);
        glBindVertexArray(VAOcontornoJanela);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);


        // =================================================
        // GRADE DA JANELA
        // =================================================

        glLineWidth(3.0f);
        glBindVertexArray(VAOgradeJanela);
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, 4);


        // =================================================
        // CHÃO
        // =================================================

        glLineWidth(8.0f);
        glBindVertexArray(VAOchao);
        glUniform4f(colorLoc, 0.0f, 0.60f, 0.10f, 1.0f);
        glDrawArrays(GL_LINES, 0, 2);


        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }


    // =================================================
    // LIMPEZA
    // =================================================

    glDeleteVertexArrays(1, &VAOcorpo);
    glDeleteVertexArrays(1, &VAOtelhado);
    glDeleteVertexArrays(1, &VAOporta);
    glDeleteVertexArrays(1, &VAOjanela);
    glDeleteVertexArrays(1, &VAOcontornoJanela);
    glDeleteVertexArrays(1, &VAOgradeJanela);
    glDeleteVertexArrays(1, &VAOchao);
    glDeleteVertexArrays(1, &VAOmacaneta);

    glDeleteProgram(shaderID);

    glfwTerminate();
    return 0;
}


// =====================================================
// CALLBACK DO TECLADO
// =====================================================

void key_callback(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


// =====================================================
// CRIAÇÃO DE VAO E VBO
// =====================================================

GLuint criaVAO(
    GLfloat *vertices,
    unsigned int tamanho)
{
    GLuint VBO;
    GLuint VAO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, tamanho, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (GLvoid *)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}


// =====================================================
// CRIA VÉRTICES DE UM CÍRCULO
// =====================================================

vector<GLfloat> criaVerticesCirculo(
    float centroX,
    float centroY,
    float raio,
    int segmentos)
{
    vector<GLfloat> vertices;

    // Centro do círculo
    vertices.push_back(centroX);
    vertices.push_back(centroY);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segmentos; i++)
    {
        float angulo = 2.0f * PI * i / segmentos;

        float x = centroX + raio * cos(angulo);
        float y = centroY + raio * sin(angulo);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }

    return vertices;
}


// =====================================================
// CONFIGURAÇÃO DOS SHADERS
// =====================================================

GLuint setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "Erro no Vertex Shader:\n" << infoLog << endl;
    }


    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "Erro no Fragment Shader:\n" << infoLog << endl;
    }


    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "Erro ao linkar Shader:\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}