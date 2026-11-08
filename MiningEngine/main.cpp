#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <numbers>
#include <random>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/shader.h"
#include "Rendering/mesh.h"
#include "Rendering/Camera.h"

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
	Mesh pyramid;
	Mesh cube;
	Mesh sphere;
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

void cleanup(GLFWwindow* window, EngineResources& resources, Shader& shader)
{
	shader.destroy();
	resources.triangle.destroy();
	resources.pyramid.destroy();
	resources.cube.destroy();

	if (window != nullptr)
		glfwDestroyWindow(window);

	glfwTerminate();
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
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
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

ShaderUniforms get_uniforms(Shader& shader)
{
	ShaderUniforms uniforms;

	uniforms.offset = shader.getUniformLocation("uOffset");
	uniforms.scale = shader.getUniformLocation("uScale");
	uniforms.rotation = shader.getUniformLocation("uRotation");
	uniforms.color = shader.getUniformLocation("uColor");
	return uniforms;
}

int main()
{
	GLFWwindow* window = createWindow(800, 600, "MiningEngine");
	if (!window)
		return -1;

	EngineResources resources;
	Shader shader;
	Camera camera;

	if (!resources.triangle.createTriangle())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	if (!resources.pyramid.createPyramid())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	if (!resources.cube.createCube())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	if (!resources.sphere.createSphere())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	

	if (!shader.load( "assets/shaders/basic3d.vert", "assets/shaders/basic.frag"))
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	ShaderUniforms uniforms = get_uniforms(shader);
	
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		resources.triangle.bind();
		
		glm::mat4 model(1.0f);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		
		float angle = static_cast<float>(glfwGetTime());

		model = glm::rotate(model,angle,glm::vec3(0.0f, 1.0f, 0.0f));
		
		glUniformMatrix4fv( shader.getUniformLocation("uModel"),1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(shader.getUniformLocation("uView"),1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shader.getUniformLocation("uProjection"),1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(uniforms.color, 1.0f, 0.7f, 0.2f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//resources.pyramid.drawIndexed();
		//resources.cube.drawIndexed();
		resources.sphere.drawIndexed();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << error << '\n';
		}
			
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	cleanup(window, resources, shader);
	return 0;
}
