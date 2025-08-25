#include "Window.h"



Window::Window(const WindowConfig& config) : window(nullptr) {

	windowWidth = config.width;
	windowHeight = config.height;

	initializeWindow(config);

}

void Window::terminate() {

	if (window != nullptr) {
		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
}

Window::~Window() {
	terminate();
}

void Window::initializeWindow(const WindowConfig& config) {

	// Initialize GLFW

	if (!glfwInit()) {
		terminate();
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.GLmajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.GLminorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window

	window = glfwCreateWindow(windowWidth, windowHeight, config.title, nullptr, nullptr);


	if (!window) {
		terminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);

	// Load OpenGL functions using GLAD

	if (!gladLoaderLoadGL()) {
		terminate();
		throw std::runtime_error("Failed to initialize GLAD");
	}

}

bool Window::windowShouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::swapBuffers() {
	glfwSwapBuffers(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}