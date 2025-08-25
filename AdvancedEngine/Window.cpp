#include "Window.h"
#include <iostream>

// Callbacks

void frameback_size_callback(GLFWwindow* window, int width, int height) {
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));



	if (win && win->onResize) {

		std::cout << "Window size changed to: " << width << ", " << height << std::endl;

		win->windowWidth = width;
		win->windowHeight = height;
		win->onResize();
	}
	else {
		std::cout << "Window size changed but no callback registered." << std::endl;
	}
}

// Class

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

	glfwSetWindowUserPointer(window, this);


	if (!window) {
		terminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, frameback_size_callback);

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

int Window::getKeyPressed(int key) {
	return glfwGetKey(window, key);
}

void Window::setWindowTitle(const char* title) {
	glfwSetWindowTitle(window, title);
}