
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "matrix.h"
#include "view_window.h"
#include "math.h"
#include "mesh.h"
#include "misc.h"

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH 960 

class inputViewWindow : public BaseViewWindow {
protected:
	vec3 _motion_dir = { 0,0,0 };

	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");

		//graph
		Mesh graph(Surface([](float s, float t) {
			std::complex<double> z = (double)(s)*exp(((double)t)*1i);
			std::complex<double> f = z*z;

			return 10.0f*vec3{(float)real(f),(float)imag(z),(float)imag(f)};

			}, 1.0f, 2*PI), 1, 100, 100);
		graph.initBuffers(GL_STREAM_DRAW);

		//normal
		Mesh disk(Surface([](float s, float t) {
			std::complex<double> z = (double)s*exp(((double)t)*1i);

			return 10.0f*vec3{(float)real(z),0,(float)imag(z)};

		}, 1.0f, 2*PI), 1, 100, 100);

		disk.initBuffers(GL_STREAM_DRAW);



		mat3 R = rotatexy<GLfloat>(PI / 2048);

		auto movtorus = [&]() {
			graph.transform_lin(rotateyz<GLfloat>(PI / 12));
			return;
		};

		_key_manager.mapKeyFunc(GLFW_KEY_R, GLFW_PRESS, movtorus);

		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			disk.draw(_main_shader);
			graph.draw(_main_shader);

			_cam.translate(_motion_dir * 0.01);

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}

		_key_manager.unmapKey(GLFW_KEY_R, GLFW_PRESS);
	}

	void _mapMovementKeys() {
		//map_keys is to condense things
		auto map_keys = [this](int action) {
			//mval determines whether motion in a direction should start or stop
			int mval = (action == GLFW_PRESS) - (action == GLFW_RELEASE);
			vec3& dir = this->_cam_manager.motion_dir;
			this->_key_manager.mapKeyFunc(GLFW_KEY_W, action, [&dir, mval]() {dir[0][2] += mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_A, action, [&dir, mval]() {dir[0][0] += mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_S, action, [&dir, mval]() {dir[0][2] += -mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_D, action, [&dir, mval]() {dir[0][0] += -mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_LEFT_SHIFT, action, [&dir, mval]() {dir[0][1] += -mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_SPACE, action, [&dir, mval]() {dir[0][1] += mval; });
		};
		map_keys(GLFW_PRESS);
		map_keys(GLFW_RELEASE);
		return;
	}
public:
	inputViewWindow(int width, int height) : BaseViewWindow(width, height) {
		_mapMovementKeys();
	}
};


int main() {
	std::cout << "Welcome.\n\n";

	inputViewWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

	std::string command;

	while (true) {
		std::cin >> command;

		if (command == "exit") {
			window.close();
			return 0;
		}

		else if (command == "launch" && !window.isRunning()) window.launch("test", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else std::cout << "invalid command\n";

	}

	return 0;
}
