#include "view_window.h"

BaseViewWindow::BaseViewWindow(
	int width,
	int height
) : _width(width), _height(height) {
	_cam = Camera(
		vec3({ 0,0,0 }), 
		vec3({ 0,0,-1 }), 
		PI / 3,
		_width,
		_height);
}

void BaseViewWindow::launch(const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	_main_thread = std::thread(&BaseViewWindow::_launchProcess, this, title, monitor, share);
	//_main_thread.detach();
}

void BaseViewWindow::close()
{
	_main_thread.detach();

	return;
}

void BaseViewWindow::_launchProcess(const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
	}

	_window = glfwCreateWindow(_width, _height, title, monitor, share);
	if (!_window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
	}
	glfwMakeContextCurrent(_window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Renderer: %s\n", _renderer);
	printf("OpenGL version supported %s\n", _version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	_main();

	return;
}



