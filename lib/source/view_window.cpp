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
	_is_running = true;
	_main_thread = std::thread(&BaseViewWindow::_windowProgram, this, title, monitor, share);
}

void BaseViewWindow::close()
{
	glfwSetWindowShouldClose(_window, true);
	return;
}

void BaseViewWindow::_windowProgram(const char* title, GLFWmonitor* monitor, GLFWwindow* share)
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

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, _keyCallback);

	const GLubyte* _renderer = glGetString(GL_RENDERER);
	const GLubyte* _version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", _renderer);
	printf("OpenGL version supported %s\n", _version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	_main();

	_main_thread.detach();

	_is_running = false;
	glfwDestroyWindow(_window);
	glfwTerminate();
	return;
}

void BaseViewWindow::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto win = static_cast<BaseViewWindow*>(glfwGetWindowUserPointer(window));

	win->_key_manager.callKeyFunc(key, action);
}

void KeyManager::callKeyFunc(int key, int action)
{
	pair<int, int> keyaction({ key,action });

	if (keymap.contains(keyaction)) keymap.at(keyaction)();

	return;
}
