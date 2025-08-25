#pragma once

#include "Window.h"
#include "Mesh.h"
#include "Material.h"



class Engine {
public:
	Engine();
	~Engine();

	void run();

private:

	void tick();
	void render();

	void initialize();
	void initializeDebuggingObjects();

	Window* window;

	const unsigned int currentWidth;
	const unsigned int currentHeight;

	// DEBUG

	Mesh* mesh;
	Material* material;
};