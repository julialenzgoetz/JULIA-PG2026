/*
 * Hello Triangle - Código adaptado de:
 *   - https://learnopengl.com/#!Getting-started/Hello-Triangle
 *   - https://antongerdelan.net/opengl/glcontext2.html
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// ============================================================
// CONTROLE DO EXERCICIO
// ============================================================

// 1 = Questao 1a - preenchido
// 2 = Questao 1b - contorno
// 3 = Questao 1c - pontos
// 4 = Questao 1d - todas juntas
int currentMode = 1;


// Protótipo da função de callback de teclado
void key_callback(
	GLFWwindow *window,
	int key,
	int scancode,
	int action,
	int mode
);

// Protótipos das funções
int setupShader();
int setupGeometry();


// Dimensões da janela
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;


// ============================================================
// VERTEX SHADER
// ============================================================

const GLchar *vertexShaderSource = R"glsl(
 #version 400

 layout (location = 0) in vec3 position;

 void main()
 {
	 gl_Position = vec4(
		 position.x,
		 position.y,
		 position.z,
		 1.0
	 );
 }
)glsl";


// ============================================================
// FRAGMENT SHADER
// ============================================================

const GLchar *fragmentShaderSource = R"glsl(
 #version 400

 uniform vec4 inputColor;

 out vec4 color;

 void main()
 {
	 color = inputColor;
 }
)glsl";


// ============================================================
// MAIN
// ============================================================

int main()
{
	// Inicialização da GLFW
	glfwInit();


	// Estas linhas permanecem comentadas porque alguns
	// computadores não aceitam a configuração forçada.

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Criação da janela
	GLFWwindow *window =
		glfwCreateWindow(
			WIDTH,
			HEIGHT,
			"Lista 1 - Questao 1",
			nullptr,
			nullptr
		);


	if (!window)
	{
		cerr
			<< "Falha ao criar a janela GLFW"
			<< endl;

		glfwTerminate();

		return -1;
	}


	glfwMakeContextCurrent(window);


	// Callback do teclado
	glfwSetKeyCallback(
		window,
		key_callback
	);


	// ========================================================
	// GLAD
	// ========================================================

	if (!gladLoadGLLoader(
			(GLADloadproc)glfwGetProcAddress))
	{
		cerr
			<< "Falha ao inicializar GLAD"
			<< endl;

		return -1;
	}


	// Informações da OpenGL
	const GLubyte *renderer =
		glGetString(GL_RENDERER);

	const GLubyte *version =
		glGetString(GL_VERSION);


	cout
		<< "Renderer: "
		<< renderer
		<< endl;

	cout
		<< "OpenGL version supported: "
		<< version
		<< endl;


	// ========================================================
	// VIEWPORT
	// ========================================================

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


	// ========================================================
	// SHADER
	// ========================================================

	GLuint shaderID =
		setupShader();


	// ========================================================
	// GEOMETRIA
	// ========================================================

	GLuint VAO =
		setupGeometry();


	// Localização da variável de cor no shader
	GLint colorLoc =
		glGetUniformLocation(
			shaderID,
			"inputColor"
		);


	glUseProgram(shaderID);


	// ========================================================
	// MENU
	// ========================================================

	cout << endl;

	cout
		<< "=============================="
		<< endl;

	cout
		<< " LISTA 1 - QUESTAO 1"
		<< endl;

	cout
		<< "=============================="
		<< endl;

	cout
		<< "1 - a) Poligono preenchido"
		<< endl;

	cout
		<< "2 - b) Apenas contorno"
		<< endl;

	cout
		<< "3 - c) Apenas pontos"
		<< endl;

	cout
		<< "4 - d) As tres formas juntas"
		<< endl;

	cout
		<< "ESC - Sair"
		<< endl;

	cout
		<< "=============================="
		<< endl;


	// ========================================================
	// GAME LOOP
	// ========================================================

	while (!glfwWindowShouldClose(window))
	{
		// Verifica teclado, mouse etc.
		glfwPollEvents();


		// Atualiza viewport caso a janela seja redimensionada
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


		// Fundo preto
		glClearColor(
			0.0f,
			0.0f,
			0.0f,
			1.0f
		);

		glClear(GL_COLOR_BUFFER_BIT);


		// Tamanho das linhas e pontos
		glLineWidth(10.0f);
		glPointSize(20.0f);


		glUseProgram(shaderID);

		glBindVertexArray(VAO);


		// ====================================================
		// ESCOLHA DO ITEM DO EXERCICIO
		// ====================================================

		switch (currentMode)
		{

		// ====================================================
		// 1 - QUESTAO 1a
		// APENAS POLIGONO PREENCHIDO
		// ====================================================

		case 1:

			// Magenta
			glUniform4f(
				colorLoc,
				1.0f,
				0.0f,
				1.0f,
				1.0f
			);


			glDrawArrays(
				GL_TRIANGLES,
				0,
				6
			);

			break;


		// ====================================================
		// 2 - QUESTAO 1b
		// APENAS CONTORNO
		// ====================================================

		case 2:

			// Amarelo
			glUniform4f(
				colorLoc,
				1.0f,
				1.0f,
				0.0f,
				1.0f
			);


			// Primeiro triângulo
			glDrawArrays(
				GL_LINE_LOOP,
				0,
				3
			);


			// Segundo triângulo
			glDrawArrays(
				GL_LINE_LOOP,
				3,
				3
			);

			break;


		// ====================================================
		// 3 - QUESTAO 1c
		// APENAS PONTOS
		// ====================================================

		case 3:

			// Ciano
			glUniform4f(
				colorLoc,
				0.0f,
				1.0f,
				1.0f,
				1.0f
			);


			glDrawArrays(
				GL_POINTS,
				0,
				6
			);

			break;


		// ====================================================
		// 4 - QUESTAO 1d
		// AS TRES FORMAS JUNTAS
		// ====================================================

		case 4:

			// ----------------------------------------------
			// Polígonos preenchidos - magenta
			// ----------------------------------------------

			glUniform4f(
				colorLoc,
				1.0f,
				0.0f,
				1.0f,
				1.0f
			);


			glDrawArrays(
				GL_TRIANGLES,
				0,
				6
			);


			// ----------------------------------------------
			// Contorno - amarelo
			// ----------------------------------------------

			glUniform4f(
				colorLoc,
				1.0f,
				1.0f,
				0.0f,
				1.0f
			);


			glDrawArrays(
				GL_LINE_LOOP,
				0,
				3
			);


			glDrawArrays(
				GL_LINE_LOOP,
				3,
				3
			);


			// ----------------------------------------------
			// Pontos - ciano
			// ----------------------------------------------

			glUniform4f(
				colorLoc,
				0.0f,
				1.0f,
				1.0f,
				1.0f
			);


			glDrawArrays(
				GL_POINTS,
				0,
				6
			);

			break;
		}


		glBindVertexArray(0);


		// Exibe frame
		glfwSwapBuffers(window);
	}


	// ========================================================
	// LIMPEZA
	// ========================================================

	glDeleteVertexArrays(
		1,
		&VAO
	);


	glDeleteProgram(
		shaderID
	);


	glfwTerminate();


	return 0;
}


// ============================================================
// TECLADO
// ============================================================

void key_callback(
	GLFWwindow *window,
	int key,
	int scancode,
	int action,
	int mode
)
{
	if (action == GLFW_PRESS)
	{
		// ESC
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(
				window,
				GL_TRUE
			);
		}


		// ====================================================
		// TECLA 1 - QUESTAO 1a
		// ====================================================

		if (key == GLFW_KEY_1)
		{
			currentMode = 1;

			cout
				<< "1a - Poligono preenchido"
				<< endl;
		}


		// ====================================================
		// TECLA 2 - QUESTAO 1b
		// ====================================================

		if (key == GLFW_KEY_2)
		{
			currentMode = 2;

			cout
				<< "1b - Apenas contorno"
				<< endl;
		}


		// ====================================================
		// TECLA 3 - QUESTAO 1c
		// ====================================================

		if (key == GLFW_KEY_3)
		{
			currentMode = 3;

			cout
				<< "1c - Apenas pontos"
				<< endl;
		}


		// ====================================================
		// TECLA 4 - QUESTAO 1d
		// ====================================================

		if (key == GLFW_KEY_4)
		{
			currentMode = 4;

			cout
				<< "1d - As tres formas juntas"
				<< endl;
		}
	}
}


// ============================================================
// CONFIGURACAO DO SHADER
// ============================================================

int setupShader()
{
	// ========================================================
	// VERTEX SHADER
	// ========================================================

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


	glCompileShader(
		vertexShader
	);


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
			<< "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
			<< infoLog
			<< endl;
	}


	// ========================================================
	// FRAGMENT SHADER
	// ========================================================

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


	glCompileShader(
		fragmentShader
	);


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
			<< "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
			<< infoLog
			<< endl;
	}


	// ========================================================
	// SHADER PROGRAM
	// ========================================================

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


	glLinkProgram(
		shaderProgram
	);


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
			<< "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
			<< infoLog
			<< endl;
	}


	glDeleteShader(
		vertexShader
	);


	glDeleteShader(
		fragmentShader
	);


	return shaderProgram;
}


// ============================================================
// CONFIGURACAO DA GEOMETRIA
// ============================================================

int setupGeometry()
{
	// Dois triângulos
	GLfloat vertices[] =
	{
		// x      y      z

		// T0
		-0.5f,  0.5f,  0.0f,   // v0
		-0.5f, -0.5f,  0.0f,   // v1
		 0.0f,  0.0f,  0.0f,   // v2

		// T1
		 0.0f,  0.0f,  0.0f,   // v3
		 0.5f, -0.5f,  0.0f,   // v4
		 0.5f,  0.5f,  0.0f    // v5
	};


	GLuint VBO;
	GLuint VAO;


	// ========================================================
	// VBO
	// ========================================================

	glGenBuffers(
		1,
		&VBO
	);


	glBindBuffer(
		GL_ARRAY_BUFFER,
		VBO
	);


	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW
	);


	// ========================================================
	// VAO
	// ========================================================

	glGenVertexArrays(
		1,
		&VAO
	);


	glBindVertexArray(
		VAO
	);


	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(GLvoid *)0
	);


	glEnableVertexAttribArray(
		0
	);


	glBindBuffer(
		GL_ARRAY_BUFFER,
		0
	);


	glBindVertexArray(
		0
	);


	return VAO;
}