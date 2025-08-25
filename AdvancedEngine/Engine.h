#pragma once

#include "Window.h"
#include "Mesh.h"
#include "Camera.h"
#include "WorldObject.h"



class Engine {
public:
	Engine();
	~Engine();

	void run();

	void onWindowResize();

private:

	void tick();
	void render();

	void initialize();
	void initializeDebuggingObjects();
	void initializeCamera();

	void handleCameraInput();
	void registerEvents();

	Window* window;

	const unsigned int currentWidth;
	const unsigned int currentHeight;

	// DEBUG

	WorldObject* worldObject;
	Material* material;
	Camera* camera;
};