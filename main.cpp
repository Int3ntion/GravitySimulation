#include <iostream>
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <gl/gl.h>
#include <cmath>
#include <vector>
#include <gl/glu.h>

double screenHeight = 1200.0f;
double screenWidth = 1920.0f;


const double G = 6.6743 * pow(10, -11);
const double dt = 0.00001;
const double PI = 3.141592653589;

GLFWwindow* StartGLFW();

class Object {
public:
	std::vector<double> position;
	std::vector<double> velocity;
	double radius;
	double mass;
	std::vector<double> color;

	Object(std::vector<double> position, std::vector<double> velocity, double mass, double radius = 20.0f, std::vector<double> color = { 1.0f, 0.0f, 0.0f }) {
		this->position = position;
		this->velocity = velocity;
		this->radius = radius;
		this->mass = mass;
		this->color = color;
	}

	void accelerate(double x, double y, double z) {
		this->velocity[0] += x;
		this->velocity[1] += y;
		this->velocity[2] += z;
	};
	void updatePos() {
		this->position[0] += this->velocity[0] * dt;
		this->position[1] += this->velocity[1] * dt;
		this->position[2] += this->velocity[2] * dt;
	};
	void DrawSphere() {
		glColor3f(color[0], color[1], color[2]);
		const int slices = 32, stacks = 32;
		const float radius = static_cast<float>(this->radius);

		for (int i = 0; i < stacks; ++i) {
			float lat0 = PI * (-0.5f + (float)i / stacks);
			float z0 = sin(lat0);
			float r0 = cos(lat0);

			float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);
			float z1 = sin(lat1);
			float r1 = cos(lat1);

			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j <= slices; ++j) {
				float lng = 2 * PI * (float)j / slices;
				float x = cos(lng);
				float y = sin(lng);

				glVertex3f(
					this->position[0] + radius * x * r0,
					this->position[1] + radius * y * r0,
					this->position[2] + radius * z0);
				glVertex3f(
					this->position[0] + radius * x * r1,
					this->position[1] + radius * y * r1,
					this->position[2] + radius * z1);
			}
			glEnd();
		}
		//glBegin(GL_TRIANGLE_FAN);
		//glVertex2d(position[0], position[1]);

		//for (int i = 0; i <= 50; ++i) {
		//	double angle = 2.0f * 3.141592653589 * (static_cast<float>(i) / 50);
		//	double x = position[0] + cos(angle) * radius;
		//	double y = position[1] + sin(angle) * radius;
		//	glVertex2d(x, y);
		//}

		//glEnd();
	}
};

int main() {
	GLFWwindow* window = StartGLFW();

	double centerX = screenWidth / 2.0f;
	double centerY = screenHeight / 2.0f;

	std::vector<Object> objects = {
		Object(std::vector<double>{300.0f, 300.0f, 0.0f}, std::vector<double>{50000.0f, 0.0f, 400000.0f}, 5.972e24, 50.0f, {1.0f, 0.4f, 0.0f}),
		Object(std::vector<double>{-300.0f, -300.0f, 0.0f}, std::vector<double>{-50000.0f, 0.0f, -400000.0f}, 5.972e24, 50.0f),
		//Object(std::vector<double>{-400.0f, 400.0f, 0.0f}, std::vector<double>{0.0f, 0.0f, 500000.0f}, 5.972e24, 50.0f, {1.0f, 0.6f, 0.0f}),
		Object(std::vector<double>{400.0f, -400.0f, 0.0f}, std::vector<double>{0.0f, 0.0f, -600000.0f}, 5.972e24, 50.0f, {0.0f, 0.4f, 0.8f})
	};

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& obj : objects) {
			
			for (auto& obj2 : objects) {
				if (&obj2 == &obj) { continue; };
				double dx = obj2.position[0] - obj.position[0];
				double dy = obj2.position[1] - obj.position[1];
				double dz = obj2.position[2] - obj.position[2];
				double distance = sqrt(dx * dx + dy * dy + dz * dz);
				if (distance < 150) distance = 150;
				std::vector<double> direction = { dx / distance, dy / distance, dz / distance };

				double acc1 = G * obj2.mass / (distance * distance);
				std::vector<double> acc = {
					acc1 * direction[0],
					acc1 * direction[1],
					acc1 * direction[2]
				};
				obj.accelerate(acc[0] * dt, acc[1] * dt, acc[2] * dt);
			}

			obj.updatePos();
			obj.DrawSphere();
			//if (obj.position[1] < 0 || obj.position[1] > screenHeight) {
			//	obj.velocity[1] *= -0.95;
			//}
			//if (obj.position[0] < 0 || obj.position[0] > screenWidth) {
			//	obj.velocity[0] *= -0.95;
			//}
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
	gluPerspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 10000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2000.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(1);
	return window;
}