#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

struct WindowConfig {
	unsigned int width;
	unsigned int height;
	const char* title;
	unsigned int GLmajorVersion;
	unsigned int GLminorVersion;
};

class Window {
public:
	Window(const WindowConfig& config);
	~Window();

	void swapBuffers();
	void pollEvents();

	unsigned int windowWidth;
	unsigned int windowHeight;

	bool windowShouldClose();

private:

	void initializeWindow(const WindowConfig& config);
	void terminate();


	GLFWwindow* window;

};