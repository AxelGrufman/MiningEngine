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

struct Mesh
{
	GLuint VAO = 0;
	GLuint VBO = 0;
};

struct ShaderUniforms
{
	GLint offset = -1;
	GLint scale = -1;
	GLint rotation = -1;
	GLint color = -1;
};

struct EngineResources
{
	Mesh triangle;

	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLuint shaderProgram = 0;
};

void  drawTriangle(const ShaderUniforms& uniforms, const DrawSettings& settings)
{
	glUniform2f(uniforms.offset, settings.x, settings.y);
	glUniform2f(uniforms.scale, settings.scaleX, settings.scaleY);
	glUniform1f(uniforms.rotation, settings.rotation);
	glUniform3f(uniforms.color, settings.red, settings.green, settings.blue);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void  drawRectangle(const ShaderUniforms& uniform, const DrawSettings& settings)
{
	drawTriangle(uniform, settings);
	DrawSettings secondHalf = settings;

	secondHalf.rotation += std::numbers::pi_v<float>;
	drawTriangle(uniform, secondHalf);
}

bool loadShaderSource(const std::string& path, std::string& output)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "Failed to open shader: " << path << '\n';
		return false;
	}

	output.clear();

	std::string line;

	while (std::getline(file, line))
	{
		output += line;
		output += '\n';
	}

	return true;
}

GLuint compileShader(GLenum shaderType, const std::string& shaderSource)
{
	GLuint shader = glCreateShader(shaderType);
	const char* source = shaderSource.c_str();
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	GLint compileStatus = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		std::cerr << "Shader compilation failed!\n";
		GLchar infoLog[512] = {};
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << infoLog << '\n';
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
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
		glDeleteProgram(shaderProgram);
		return 0;
	}
	return shaderProgram;
}

Mesh createTriangleMesh()
{
	Mesh mesh;
	
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	
	glGenBuffers(1, &mesh.VBO);

	
	glGenVertexArrays(1, &mesh.VAO);

	glBindVertexArray(mesh.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return mesh;
}

void cleanup(GLFWwindow* window, EngineResources& resources)
{
	if (resources.vertexShader != 0)
		glDeleteShader(resources.vertexShader);

	if (resources.fragmentShader != 0)
		glDeleteShader(resources.fragmentShader);

	if (resources.shaderProgram != 0)
		glDeleteProgram(resources.shaderProgram);

	if (resources.triangle.VBO != 0)
		glDeleteBuffers(1, &resources.triangle.VBO);

	if (resources.triangle.VAO != 0)
		glDeleteVertexArrays(1, &resources.triangle.VAO);

	if (window != nullptr)
		glfwDestroyWindow(window);

	glfwTerminate();

	resources = {};
}

GLFWwindow* createWindow(int width, int height, const char* title)
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return nullptr;
	}	
	GLFWwindow* window;
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	
	if (!window)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD!\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}
	return window;
}

void ChangeRoofColour(GLFWwindow* window, bool& wasSpacePressed, DrawSettings& roofSettings, std::mt19937& randomGenerator)
{
	std::uniform_real_distribution<float> randomColor(0.0f, 1.0f);

	bool isSpacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

	if (isSpacePressed && !wasSpacePressed)
	{
		roofSettings.red = randomColor(randomGenerator);
		roofSettings.green = randomColor(randomGenerator);
		roofSettings.blue = randomColor(randomGenerator);
	}

	wasSpacePressed = isSpacePressed;
}


void RemoveShaders(EngineResources& resources)
{
	glDetachShader(resources.shaderProgram, resources.vertexShader);
	glDetachShader(resources.shaderProgram, resources.fragmentShader);
	glDeleteShader(resources.vertexShader);
	glDeleteShader(resources.fragmentShader);
	resources.vertexShader = 0;
	resources.fragmentShader = 0;
}

ShaderUniforms get_uniforms(EngineResources resources)
{
	ShaderUniforms uniforms;

	uniforms.offset = glGetUniformLocation(resources.shaderProgram, "uOffset");
	uniforms.scale = glGetUniformLocation(resources.shaderProgram, "uScale");
	uniforms.rotation = glGetUniformLocation(resources.shaderProgram, "uRotation");
	uniforms.color = glGetUniformLocation(resources.shaderProgram, "uColor");
	return uniforms;
}

int main()
{
	GLFWwindow* window = createWindow(800, 600, "MiningEngine");
	if (!window)
		return -1;

	EngineResources resources;
	resources.triangle = createTriangleMesh();
	
	std::string vertexSource;
	std::string fragmentSource;

	bool vertexLoaded = loadShaderSource("assets/shaders/basic.vert",vertexSource);
	bool fragmentLoaded = loadShaderSource("assets/shaders/basic.frag",fragmentSource);
	
	if (!vertexLoaded || !fragmentLoaded)
	{
		cleanup(window, resources);
		return -1;
	}
		
	resources.vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
	resources.fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
	
	if (resources.vertexShader == 0 || resources.fragmentShader == 0)
	{
		cleanup(window, resources);
		return -1;
	}
		
	resources.shaderProgram = createShaderProgram(resources.vertexShader, resources.fragmentShader);
	
	if (resources.shaderProgram == 0)
	{
		cleanup(window, resources);
		return -1;
	}
	
	RemoveShaders(resources);
	
	ShaderUniforms uniforms = get_uniforms(resources);
	
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
	bool wasSpacePressed = false;
	
	while (!glfwWindowShouldClose(window))
	{
		ChangeRoofColour(window, wasSpacePressed, Roof,randomGenerator);
		
		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(resources.shaderProgram);
		glBindVertexArray(resources.triangle.VAO);
		
		drawRectangle(uniforms, Housebody);
		drawTriangle(uniforms, Roof);
		drawRectangle(uniforms, Door);
		drawRectangle(uniforms, Window1);
		drawRectangle(uniforms, Window2);
		
		GLenum error;

		while ((error = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << error << '\n';
		}
			
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	cleanup(window, resources);
	return 0;
}
