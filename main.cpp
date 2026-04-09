#include <iostream>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <cmath>
#include <vector>

double screenHeight = 800.0f;
double screenWidth = 800.0f;

GLFWwindow* StartGLFW();
void DrawCircle(double centerX, double centerY, double radius, int res);

//class Object {
//
//};

int main() {
	GLFWwindow* window = StartGLFW();

	double centerX = screenWidth / 2.0f;
	double centerY = screenHeight / 2.0f;
	double radius = 70.0f;
	int res = 100;

	std::vector<double> position = { 400.0f, 600.0f };
	std::vector<double> velocity = { 0.0f, 0.0f };	

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		DrawCircle(position[0], position[1], 50.0f, 50);

		position[0] += velocity[0];
		position[1] += velocity[1];
		velocity[1] += -9.81 / 7.0f;
		velocity[0] += 9 / 7.0f;

		if (position[1] < 0 || position[1] > screenHeight) {
			velocity[1] *= -0.95;
		}
		if (position[0] < 0 || position[0] > screenWidth) {
			velocity[0] *= -0.95;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

GLFWwindow* StartGLFW() {
	if (!glfwInit()) {
		std::cerr << "failed to initialize glfw, panic!" << std::endl;
		return nullptr;
	}
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "gravity_sim", NULL, NULL);
	glfwMakeContextCurrent(window);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glfwSwapInterval(1);
	return window;
}

void DrawCircle(double centerX, double centerY, double radius, int res) {
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(centerX, centerY);

	for (int i = 0; i <= res; ++i) {
		double angle = 2.0f * 3.141592653589 * (static_cast<float>(i) / res);
		double x = centerX + cos(angle) * radius;
		double y = centerY + sin(angle) * radius;
		glVertex2d(x, y);
	}

	glEnd();
}