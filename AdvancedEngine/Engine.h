#pragma once

#include "Window.h"
#include "Mesh.h"
#include "Camera.h"
#include "WorldObject.h"
#include "ChunksManager.h"


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
	void initializeWorld();

	void handleCameraInput();
	void registerEvents();

	Window* window;

	unsigned int currentWidth;
	unsigned int currentHeight;

	float deltaTime;

	// DEBUG

	Camera* camera;
	ChunksManager* chunksManager;

	double lastX;
	double lastY;
};