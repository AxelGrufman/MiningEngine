#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <numbers>
#include <random>

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
	
	DrawSettings Housebody;
	Housebody.x = 0.0f;
	Housebody.y = -0.1f;
	Housebody.scaleX = 1.0f;
	Housebody.scaleY = 0.7f;
	Housebody.red = 0.0f;
	Housebody.green = 0.0f;
	Housebody.blue = 0.0f;
	
	DrawSettings Roof;
	Roof.x = 0.0f;
	Roof.y = 0.45f;
	Roof.scaleX = 1.3f;
	Roof.scaleY = 0.4f;
	Roof.red = 1.0f;
	Roof.green = 0.0f;
	Roof.blue = 0.0f;
	
	DrawSettings Door;
	Door.x = 0.0f;
	Door.y = -0.32f;
	Door.scaleX = 0.15f;
	Door.scaleY = 0.25f;
	Door.red = 0.0f;
	Door.green = 1.0f;
	Door.blue = 0.0f;
	
	DrawSettings Window1;
	Window1.x = -0.3f;
	Window1.y =	 0.0f;
	Window1.scaleX = 0.2f;
	Window1.scaleY = 0.2f;
	Window1.red = 0.0f;
	Window1.green = 0.0f;
	Window1.blue = 1.0f;
	
	DrawSettings Window2;
	Window2.x = 0.3f;
	Window2.y =	 0.0f;
	Window2.scaleX = 0.2f;
	Window2.scaleY = 0.2f;
	Window2.red = 0.0f;
	Window2.green = 0.0f;
	Window2.blue = 1.0f;
	
	std::random_device rd;
	std::mt19937 randomGenerator(rd());
	std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);
	
	bool wasSpacePressed = false;
	
	while (!glfwWindowShouldClose(window))
	{
		bool isSpacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

		if (isSpacePressed && !wasSpacePressed)
		{
			Roof.red = randomColor(randomGenerator);
			Roof.green = randomColor(randomGenerator);
			Roof.blue = randomColor(randomGenerator);
		}

		wasSpacePressed = isSpacePressed;
		
		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		
		drawRectangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, Housebody);
		drawTriangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, Roof);
		drawRectangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, Door);
		drawRectangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, Window1);
		drawRectangle(offsetLocation, scaleLocation, rotationLocation, colorLocation, Window2);
			
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
