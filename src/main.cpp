
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

typedef std::complex<double> C;

std::complex<double> to_complex(float a, float b) {
	return ((double)a) + ((double)b)*1i;
}


//This is a small example program where I graph part of the imaginary of a mobius
//transformation on a disk in the complex plane.
class programWindow : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");
		
		//random function
		Mesh graph(Surface([=](float s, float t) {

			return s*vec3{cos(t),0,sin(t)};

			},5, 2*PI), 69, 50, 50);

		//graph.setType(LINE);
		graph.initBuffers(GL_STREAM_DRAW);
		graph.checkChar();

		auto mobius = [=](matrix<2,2,C> M, C z) {
			return (M[0][0]*z + M[0][1])/(M[1][0]*z + M[1][1]);
		};


		//loops through face list and vertices to update positions in GPU
		//as a function of the initial vertex positions. This graphs the real
		//part of a mobius transformatio
		auto F = [&](float* mem, size_t size) {
			auto faces = graph.faces();
			int i = 0;
			float t = pow(sin(glfwGetTime()),2);
			for (Face* F: faces) {
				for (Vertex* v : F->vertexArray()) {
					C z = to_complex(*v->position[0],*v->position[2]);
					C f = mobius(mobius_mat,z);

					vec3 newpos = {(float)real(z),(float)real(f),(float)imag(z)};

					for (int k = 0; k < 3; k++) {
						mem[i] = *newpos[k];
						i++;
					}
				}	
			}
		};

		graph.transformCPU(POSITION,F);


		draw_count = graph._pointCount();
		//main loop
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			graph.draw(_main_shader,draw_count);
			graph.transformCPU(POSITION,F);
			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	
public:
	size_t draw_count = 1000;

	matrix<2,2,C> mobius_mat = {
			2,1,
			3,1
		};
	//one day something will be here
	programWindow(int width, int height) : BaseViewWindow(width, height){}
};


int main() {
	std::cout << "Welcome.\n\n";

	programWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

	std::string command;

	auto move_mobius = [&]() {
		while (true) {
			window.mobius_mat = {
			(C)10*cos(glfwGetTime()),1,
			3,(C)10*sin(glfwGetTime())
			};
		}
		return;
	};

	std::thread mobius_mover(move_mobius);

	while (true) {
		

		std::cin >> command;

		if (command == "exit") {
			window.close();
			return 0;
		}

		else if (command == "launch" && !window.isRunning()) window.launch("test", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else if (command == "drawcount") {
			int val;
			std::cin >> val;
			window.draw_count = val; 
		}

		else std::cout << "invalid command\n";

	}

	return 0;
}
