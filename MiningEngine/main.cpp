#include <iostream>
#include <GLFW/glfw3.h>

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}
	
	GLFWwindow* window;
	window = glfwCreateWindow(800, 600, "MiningEngine", nullptr, nullptr);
	
	glfwMakeContextCurrent(window);
	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		glBegin(GL_TRIANGLES);
		
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(-0.5f, 0.5f);
		glVertex2f(0.5f, 0.5f);
		
		glEnd();
		
		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);

		glfwSwapBuffers(window);
	}

	return 0;
}
