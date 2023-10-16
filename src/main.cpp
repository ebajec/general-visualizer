
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

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920 

typedef std::complex<double> C;

int DRAW_AMOUNT = 1;

class programWindow : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		
		//random function
		Mesh graph(Surface([=](float s, float t) {
			return Torus(2.0f,1.0f)(s,t);
			}, 2*PI, 2*PI), 2, 200, 150);

		//graph.setType(LINE);
		graph.initBuffers(GL_STREAM_DRAW);
	
		graph.checkChar();

		//main loop
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			graph.draw(_main_shader);
			//graph.transformAffine(rotatexz<GLfloat>(PI/2024));
			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	
public:
	//one day something will be here
	programWindow(int width, int height) : BaseViewWindow(width, height) {}
};


int main() {
	std::cout << "Welcome.\n\n";

	programWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

	std::string command;

	while (true) {
		std::cin >> command;

		if (command == "exit") {
			window.close();
			return 0;
		}

		else if (command == "launch" && !window.isRunning()) window.launch("test", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else if (command == "setcount") {
			int val;
			std::cin >> val;
			DRAW_AMOUNT = val; 
		}

		else std::cout << "invalid command\n";

	}

	return 0;
}
