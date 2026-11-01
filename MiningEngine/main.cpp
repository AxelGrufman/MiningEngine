#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <numbers>

struct DrawSettings
{
	float x = 0.0f;
	float y = 0.0f;

	float scaleX = 1.0f;
	float scaleY = 1.0f;

	float rotation = 0.0f;

	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;
};

void drawTriangle(
	GLint offsetLocation,
	GLint scaleLocation,
	GLint rotationLocation,
	GLint colorLocation,
	const DrawSettings& settings)
{
	glUniform2f(offsetLocation, settings.x, settings.y);
	glUniform2f(scaleLocation, settings.scaleX, settings.scaleY);
	glUniform1f(rotationLocation, settings.rotation);
	glUniform3f(colorLocation, settings.red, settings.green, settings.blue);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawRectangle(
	GLint offsetLocation,
	GLint scaleLocation,
	GLint rotationLocation,
	GLint colorLocation,
	const DrawSettings& settings)
{
	drawTriangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, settings);
	DrawSettings secondHalf = settings;

	secondHalf.rotation += std::numbers::pi_v<float>;
	drawTriangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, secondHalf);
}

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}
	
	GLFWwindow* window;
	window = glfwCreateWindow(800, 600, "MiningEngine", nullptr, nullptr);
	
	if (!window)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD!\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}
	
	
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW
	);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	std::ifstream shaderFile("assets/shaders/basic.vert");

	if (!shaderFile.is_open())
	{
		std::cerr << "Failed to open vertex shader!\n";
	}
	
	std::string shaderSource;
	std::string line;

	while (std::getline(shaderFile, line))
	{
		shaderSource += line;
		shaderSource += '\n';
	}
		
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	
	const char* source = shaderSource.c_str();

	glShaderSource(vertexShader, 1, &source, nullptr);
	
	glCompileShader(vertexShader);
	
	GLint compileStatus = GL_FALSE;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
	
	if (compileStatus == GL_FALSE)
	{
		std::cerr << "Vertex shader compilation failed!\n";
		GLchar infoLog[512] = {};

		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);

		std::cerr << "Shader compilation error:\n"
				  << infoLog << '\n';
	}
	
	
	//fragment shader
	
	std::ifstream FragmentFile("assets/shaders/basic.frag");

	if (!FragmentFile.is_open())
	{
		std::cerr << "Failed to open fragment shader!\n";
	}
	
	std::string fragmentSource;

	while (std::getline(FragmentFile, line))
	{
		fragmentSource += line;
		fragmentSource += '\n';
	}
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* fragmentSourcePtr = fragmentSource.c_str();

	glShaderSource(fragmentShader, 1, &fragmentSourcePtr, nullptr);

	glCompileShader(fragmentShader);
	
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);
	
	if (compileStatus == GL_FALSE)
	{
		std::cerr << "Fragment shader compilation failed!\n";
		GLchar infoLog[512] = {};
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "Shader compilation error:\n"
		<< infoLog << '\n';
	}
	
	GLint vertexStatus = GL_FALSE;
	GLint fragmentStatus = GL_FALSE;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);
	
	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	
	GLint linkStatus = GL_FALSE;
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		std::cerr << "Program linking failed!\n";
		GLchar infoLog[512] = {};

		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);

		std::cerr << infoLog << '\n';
	}
	
	GLint offsetLocation = glGetUniformLocation(shaderProgram, "uOffset");
	GLint scaleLocation = glGetUniformLocation(shaderProgram, "uScale");
	GLint rotationLocation = glGetUniformLocation(shaderProgram, "uRotation");
	GLint colorLocation = glGetUniformLocation(shaderProgram, "uColor");
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	DrawSettings firstTriangle;

	firstTriangle.x = -0.4f;
	firstTriangle.scaleX = 0.4f;
	firstTriangle.scaleY = 0.8f;

	firstTriangle.red = 1.0f;
	firstTriangle.green = 0.0f;
	firstTriangle.blue = 0.0f;
	
	DrawSettings SecondTriangel;

	SecondTriangel.x = 0.4f;
	SecondTriangel.scaleX = 0.7f;
	SecondTriangel.scaleY = 0.4f;

	SecondTriangel.red = 0.0f;
	SecondTriangel.green = 0.0f;
	SecondTriangel.blue = 1.0f;
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		
		drawRectangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, firstTriangle);
			
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
