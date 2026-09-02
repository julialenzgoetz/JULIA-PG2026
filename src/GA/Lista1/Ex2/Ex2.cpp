/*
    GA - Lista 1 - Questão 2
    Processamento Grafico

    Teclas:
    1 - Circulo
    2 - Octagono
    3 - Pentagono
    4 - Pac-Man
    5 - Fatia de pizza
    6 - Estrela
    7 - Espiral
    ESC - Sair
*/

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

const float PI = 3.14159265f;

const GLuint WIDTH = 800;
const GLuint HEIGHT = 800;

//A janela quadrada ajuda bastante neste exercício,
//Porque evita que um círculo fique visualmente esticado.

// ============================================================
// Estrutura para armazenar uma geometria
// ============================================================

struct Geometry
{
    GLuint VAO;
    GLuint VBO;

    GLsizei nVertices; //Guarda quantos vértices existem.

    GLenum primitive; //Determina como os vértices devem ser conectados.
};


// Figura selecionada
int currentShape = 1; //Variável que controla qual figura aparece na tela.

// ============================================================
// Prototipos
// ============================================================

void key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mode
);

GLuint setupShader();

Geometry createGeometry(
    const vector<GLfloat>& vertices,
    GLenum primitive
);

Geometry createCircle(
    int nPoints,
    float radius
);

Geometry createPacman(
    int nPoints,
    float radius
);

Geometry createPizza(
    int nPoints,
    float radius
);

Geometry createStar(
    int nPoints,
    float minRadius,
    float maxRadius
);

Geometry createSpiral();

// ============================================================
// SHADERS
// ============================================================

const GLchar* vertexShaderSource =
"#version 400\n"
"layout (location = 0) in vec3 position;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position, 1.0);\n"
"}\n";

const GLchar* fragmentShaderSource =
"#version 400\n"
"\n"
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"    color = inputColor;\n"
"}\n";


// ============================================================
// MAIN
// ============================================================

int main()
{
    // --------------------------------------------------------
    // Inicializacao GLFW
    // --------------------------------------------------------

    if (!glfwInit())
    {
        cout << "Erro ao inicializar GLFW." << endl;
        return -1;
    }

    // Criacao da janela
    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Lista 1 - Questao 2",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        cout << "Erro ao criar janela GLFW." << endl;

        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback); //Conecta o teclado à função.


    // --------------------------------------------------------
    // Inicializacao GLAD
    // --------------------------------------------------------

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress))
    {
        cout << "Erro ao inicializar GLAD." << endl;

        return -1;
    }

    cout << "OpenGL: "
         << glGetString(GL_VERSION)
         << endl;

    // --------------------------------------------------------
    // Shader
    // --------------------------------------------------------

    GLuint shaderID = setupShader();

    glUseProgram(shaderID);

    GLint colorLoc =
        glGetUniformLocation(
            shaderID,
            "inputColor"
        );

    // ========================================================
    // CRIACAO DAS FIGURAS DA QUESTAO 2
    // ========================================================

    // Questao 2
    // Muitos pontos = aparencia de circulo
    Geometry circle =
        createCircle(
            100,
            0.6f
        );

    // Questao 2a
    Geometry octagon =
        createCircle(
            8,
            0.6f
        );

    // Questao 2b
    Geometry pentagon =
        createCircle(
            5,
            0.6f
        );

    // Questao 2c
    Geometry pacman =
        createPacman(
            80,
            0.6f
        );

    // Questao 2d
    Geometry pizza =
        createPizza(
            40,
            0.65f
        );

    // Questao 2e
    Geometry star =
        createStar(
            5,
            0.28f,
            0.65f  //altera entre raio maior e raio menor.
        );

    // Questao 2f
    Geometry spiral =
        createSpiral(); //aumenta o ângulo e também o raio, conectando os pontos.

    cout << endl;

    cout << "==============================" << endl;
    cout << " LISTA 1 - QUESTAO 2" << endl;
    cout << "==============================" << endl;

    cout << "1 - Circulo" << endl;
    cout << "2 - Octagono" << endl;
    cout << "3 - Pentagono" << endl;
    cout << "4 - Pac-Man" << endl;
    cout << "5 - Fatia de pizza" << endl;
    cout << "6 - Estrela" << endl;
    cout << "7 - Espiral" << endl;
    cout << "ESC - Sair" << endl;

    cout << "==============================" << endl;


    // ========================================================
    // GAME LOOP
    // ========================================================

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();


        // Fundo preto
        glClearColor(
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        int width;
        int height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        glViewport(
            0,
            0,
            width,
            height
        );


        glUseProgram(shaderID);


        // ----------------------------------------------------
        // Escolha da figura
        // ----------------------------------------------------

        switch (currentShape)
        {

        // ====================================================
        // 1 - CIRCULO
        // ====================================================

        case 1:

            glUniform4f(
                colorLoc,
                0.0f,
                0.6f,
                1.0f,
                1.0f
            );

            glBindVertexArray(circle.VAO);

            glDrawArrays(
                circle.primitive,
                0,
                circle.nVertices
            );

            break;


        // ====================================================
        // 2 - OCTAGONO
        // ====================================================

        case 2:

            glUniform4f(
                colorLoc,
                0.2f,
                0.8f,
                0.3f,
                1.0f
            );

            glBindVertexArray(octagon.VAO);

            glDrawArrays(
                octagon.primitive,
                0,
                octagon.nVertices
            );

            break;


        // ====================================================
        // 3 - PENTAGONO
        // ====================================================

        case 3:

            glUniform4f(
                colorLoc,
                0.8f,
                0.2f,
                0.8f,
                1.0f
            );

            glBindVertexArray(pentagon.VAO);

            glDrawArrays(
                pentagon.primitive,
                0,
                pentagon.nVertices
            );

            break;


        // ====================================================
        // 4 - PAC-MAN
        // ====================================================

        case 4:

            glUniform4f(
                colorLoc,
                1.0f,
                0.85f,
                0.0f,
                1.0f
            );

            glBindVertexArray(pacman.VAO);

            glDrawArrays(
                pacman.primitive,
                0,
                pacman.nVertices
            );

            break;


        // ====================================================
        // 5 - FATIA DE PIZZA
        // ====================================================

        case 5:

            glUniform4f(
                colorLoc,
                1.0f,
                0.45f,
                0.0f,
                1.0f
            );

            glBindVertexArray(pizza.VAO);

            glDrawArrays(
                pizza.primitive,
                0,
                pizza.nVertices
            );

            break;


        // ====================================================
        // 6 - ESTRELA
        // ====================================================

        case 6:

            glUniform4f(
                colorLoc,
                1.0f,
                0.8f,
                0.1f,
                1.0f
            );

            glBindVertexArray(star.VAO);

            glDrawArrays(
                star.primitive,
                0,
                star.nVertices
            );

            break;


        // ====================================================
        // 7 - ESPIRAL
        // ====================================================

        case 7:

            glUniform4f(
                colorLoc,
                1.0f,
                0.2f,
                0.2f,
                1.0f
            );

            glLineWidth(4.0f);

            glBindVertexArray(spiral.VAO);

            glDrawArrays(
                spiral.primitive,
                0,
                spiral.nVertices
            );

            break;
        }


        glBindVertexArray(0);


        glfwSwapBuffers(window);
    }


    // ========================================================
    // LIMPEZA
    // ========================================================

    glDeleteVertexArrays(1, &circle.VAO);
    glDeleteBuffers(1, &circle.VBO);

    glDeleteVertexArrays(1, &octagon.VAO);
    glDeleteBuffers(1, &octagon.VBO);

    glDeleteVertexArrays(1, &pentagon.VAO);
    glDeleteBuffers(1, &pentagon.VBO);

    glDeleteVertexArrays(1, &pacman.VAO);
    glDeleteBuffers(1, &pacman.VBO);

    glDeleteVertexArrays(1, &pizza.VAO);
    glDeleteBuffers(1, &pizza.VBO);

    glDeleteVertexArrays(1, &star.VAO);
    glDeleteBuffers(1, &star.VBO);

    glDeleteVertexArrays(1, &spiral.VAO);
    glDeleteBuffers(1, &spiral.VBO);


    glDeleteProgram(shaderID);

    glfwTerminate();

    return 0;
}


// ============================================================
// TECLADO
// ============================================================

void key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mode
)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(
                window,
                GL_TRUE
            );
        }

        if (key == GLFW_KEY_1)
        {
            currentShape = 1;

            cout << "Circulo" << endl;
        }


        if (key == GLFW_KEY_2)
        {
            currentShape = 2;

            cout << "Octagono" << endl;
        }


        if (key == GLFW_KEY_3)
        {
            currentShape = 3;

            cout << "Pentagono" << endl;
        }


        if (key == GLFW_KEY_4)
        {
            currentShape = 4;

            cout << "Pac-Man" << endl;
        }


        if (key == GLFW_KEY_5)
        {
            currentShape = 5;

            cout << "Fatia de pizza" << endl;
        }


        if (key == GLFW_KEY_6)
        {
            currentShape = 6;

            cout << "Estrela" << endl;
        }


        if (key == GLFW_KEY_7)
        {
            currentShape = 7;

            cout << "Espiral" << endl;
        }
    }
}

// ============================================================
// CONFIGURACAO DO SHADER
// ============================================================

GLuint setupShader()
{
    // --------------------------------------------------------
    // Vertex Shader
    // --------------------------------------------------------

    GLuint vertexShader =
        glCreateShader(
            GL_VERTEX_SHADER
        );


    glShaderSource(
        vertexShader,
        1,
        &vertexShaderSource,
        NULL
    );

    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(
        vertexShader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        glGetShaderInfoLog(
            vertexShader,
            512,
            NULL,
            infoLog
        );

        cout
            << "ERRO NO VERTEX SHADER:\n"
            << infoLog
            << endl;
    }

    // --------------------------------------------------------
    // Fragment Shader
    // --------------------------------------------------------

    GLuint fragmentShader =
        glCreateShader(
            GL_FRAGMENT_SHADER
        );


    glShaderSource(
        fragmentShader,
        1,
        &fragmentShaderSource,
        NULL
    );


    glCompileShader(fragmentShader);


    glGetShaderiv(
        fragmentShader,
        GL_COMPILE_STATUS,
        &success
    );


    if (!success)
    {
        glGetShaderInfoLog(
            fragmentShader,
            512,
            NULL,
            infoLog
        );

        cout
            << "ERRO NO FRAGMENT SHADER:\n"
            << infoLog
            << endl;
    }

    // --------------------------------------------------------
    // Shader Program
    // --------------------------------------------------------

    GLuint shaderProgram =
        glCreateProgram();


    glAttachShader(
        shaderProgram,
        vertexShader
    );


    glAttachShader(
        shaderProgram,
        fragmentShader
    );


    glLinkProgram(shaderProgram);


    glGetProgramiv(
        shaderProgram,
        GL_LINK_STATUS,
        &success
    );


    if (!success)
    {
        glGetProgramInfoLog(
            shaderProgram,
            512,
            NULL,
            infoLog
        );

        cout
            << "ERRO AO LINKAR SHADER:\n"
            << infoLog
            << endl;
    }


    glDeleteShader(vertexShader);

    glDeleteShader(fragmentShader);


    return shaderProgram;
}

// ============================================================
// CRIACAO DO VBO E VAO
// ============================================================

Geometry createGeometry(
    const vector<GLfloat>& vertices,
    GLenum primitive
)
{
    Geometry geometry;


    geometry.nVertices =
        static_cast<GLsizei>(
            vertices.size() / 3
        );


    geometry.primitive =
        primitive;


    // --------------------------------------------------------
    // VAO
    // --------------------------------------------------------

    glGenVertexArrays(
        1,
        &geometry.VAO
    );


    glBindVertexArray(
        geometry.VAO
    );


    // --------------------------------------------------------
    // VBO
    // --------------------------------------------------------

    glGenBuffers(
        1,
        &geometry.VBO
    );


    glBindBuffer(
        GL_ARRAY_BUFFER,
        geometry.VBO
    );


    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(),
        GL_STATIC_DRAW
    );


    // --------------------------------------------------------
    // Atributo POSITION
    // --------------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (GLvoid*)0
    );


    glEnableVertexAttribArray(0);


    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );


    glBindVertexArray(0);


    return geometry;
}


// ============================================================
// QUESTAO 2
//
// CIRCULO
//
// x = r cos(theta)
// y = r sin(theta)
// ============================================================

Geometry createCircle(
    int nPoints,
    float radius
)
{
    vector<GLfloat> vertices;


    // --------------------------------------------------------
    // Primeiro vertice = centro
    // --------------------------------------------------------

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);


    float slice =
        2.0f * PI / nPoints;


    // --------------------------------------------------------
    // Pontos da circunferencia
    // --------------------------------------------------------

    for (int i = 0; i <= nPoints; i++)
    {
        float angle =
            i * slice;


        float x =
            radius * cos(angle);


        float y =
            radius * sin(angle);


        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }


    return createGeometry(
        vertices,
        GL_TRIANGLE_FAN
    );
}


// ============================================================
// QUESTAO 2C
//
// PAC-MAN
//
// Mesmo principio do circulo,
// mas nao percorremos os 360 graus completos.
// Deixamos uma abertura de 60 graus.
// ============================================================

Geometry createPacman(
    int nPoints,
    float radius
)
{
    vector<GLfloat> vertices;


    // Centro
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);


    // Boca de 60 graus
    float mouth =
        60.0f * PI / 180.0f;


    float startAngle =
        mouth / 2.0f;


    float endAngle =
        2.0f * PI - mouth / 2.0f;


    float slice =
        (endAngle - startAngle)
        / nPoints;


    for (int i = 0; i <= nPoints; i++)
    {
        float angle =
            startAngle +
            i * slice;


        float x =
            radius * cos(angle);


        float y =
            radius * sin(angle);


        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }


    return createGeometry(
        vertices,
        GL_TRIANGLE_FAN
    );
}


// ============================================================
// QUESTAO 2D
//
// FATIA DE PIZZA
//
// Centro + somente um trecho da circunferencia.
// ============================================================

Geometry createPizza(
    int nPoints,
    float radius
)
{
    vector<GLfloat> vertices;


    // Centro
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);


    // Fatia de 70 graus
    float startAngle =
        -35.0f * PI / 180.0f;


    float endAngle =
        35.0f * PI / 180.0f;


    float slice =
        (endAngle - startAngle)
        / nPoints;


    for (int i = 0; i <= nPoints; i++)
    {
        float angle =
            startAngle +
            i * slice;


        float x =
            radius * cos(angle);


        float y =
            radius * sin(angle);


        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }


    return createGeometry(
        vertices,
        GL_TRIANGLE_FAN
    );
}


// ============================================================
// QUESTAO 2E
//
// ESTRELA
//
// Alternamos:
// raio maior -> ponta
// raio menor -> parte interna
// ============================================================

Geometry createStar(
    int nPoints,
    float minRadius,
    float maxRadius
)
{
    vector<GLfloat> vertices;


    // Centro
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);


    int totalPoints =
        nPoints * 2;


    float slice =
        2.0f * PI / totalPoints;


    // Comecamos no topo
    float initialAngle =
        PI / 2.0f;


    for (int i = 0; i <= totalPoints; i++)
    {
        float radius;


        if (i % 2 == 0)
        {
            radius =
                maxRadius;
        }
        else
        {
            radius =
                minRadius;
        }


        float angle =
            initialAngle +
            i * slice;


        float x =
            radius * cos(angle);


        float y =
            radius * sin(angle);


        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }


    return createGeometry(
        vertices,
        GL_TRIANGLE_FAN
    );
}


// ============================================================
// QUESTAO 2F
//
// ESPIRAL
//
// O angulo aumenta
// E o raio tambem aumenta progressivamente.
// ============================================================

Geometry createSpiral()
{
    vector<GLfloat> vertices;


    float radius =
        0.02f;


    float slice =
        0.08f;


    for (
        float angle = 0.0f;
        angle < 10.0f * PI;
        angle += slice
        )
    {
        float x =
            radius * cos(angle);


        float y =
            radius * sin(angle);


        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);


        // Faz a espiral crescer
        radius += 0.0013f;
    }


    return createGeometry(
        vertices,
        GL_LINE_STRIP
    );
}