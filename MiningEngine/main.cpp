#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <filesystem>


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
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
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
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		
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
