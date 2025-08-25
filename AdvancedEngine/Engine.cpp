#include "Engine.h"
#include "MoreMaterials.h"
#include <chrono>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
const char* vertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core

out vec4 FragColor;

void main() {
	FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color
}

)";


Engine::Engine() : currentWidth(WIDTH), currentHeight(HEIGHT) {

	WindowConfig config;
	config.width = currentWidth;
	config.height = currentHeight;
	config.GLmajorVersion = 4;
	config.GLminorVersion = 6;
	config.title = "Not advanced engine";

	window = new Window(config);

	initialize();


};

Engine::~Engine() {
	delete window;
}

void Engine::handleCameraInput() {


	if (window->getKeyPressed(GLFW_KEY_W) == GLFW_PRESS) {
		camera->position += camera->direction * 0.1f;
		camera->recomputeMatrices();
	}
	if (window->getKeyPressed(GLFW_KEY_A) == GLFW_PRESS) {
		camera->position += camera->cameraRight * -0.1f;
		camera->recomputeMatrices();
	}
	if (window->getKeyPressed(GLFW_KEY_S) == GLFW_PRESS) {
		camera->position += camera->direction * -0.1f;
		camera->recomputeMatrices();
	}
	if (window->getKeyPressed(GLFW_KEY_D) == GLFW_PRESS) {
		camera->position += camera->cameraRight * 0.1f;
		camera->recomputeMatrices();
	}
	
}

void Engine::onWindowResize() {
	glViewport(0, 0, window->windowWidth, window->windowHeight);

	camera->aspectRatio = static_cast<float>(window->windowWidth) / static_cast<float>(window->windowHeight);
	camera->recomputeMatrices();
}

void Engine::initialize() {
	// Initialization code can go here if needed

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set a clear color
	glViewport(0, 0, currentWidth, currentHeight);
	initializeCamera();
	initializeDebuggingObjects();
	
	registerEvents();
}

void Engine::initializeCamera() {

	camera = new Camera(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1), 90.f);
	camera->aspectRatio = static_cast<float>(window->windowWidth) / static_cast<float>(window->windowHeight);

	camera->recomputeMatrices();

}

void Engine::registerEvents() {
	window->onResize = std::bind(&Engine::onWindowResize, this);
}

void Engine::initializeDebuggingObjects() {

	/*VertexAttribute positionVA = VertexAttribute{
		.sizeInBytes = sizeof(float) * 3,
		.size = 3,
		.type = GL_FLOAT,
		.normalized = GL_FALSE
	};*/

	std::vector<float> vertices = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	std::vector<unsigned int> indices = {
		0, 1, 3,
		// first triangle
	  1, 2, 3    // second triangle
	};

	material = new TransformMaterial(fragmentShaderSource);
	Mesh* mesh = new Mesh(vertices, indices, material);

	
	worldObject = new WorldObject(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), mesh, camera);

}

void Engine::tick() {
	handleCameraInput();
}

void Engine::render() {

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldObject->render();
}

void Engine::run() {

	int fpsCounter = 0;
	auto start = std::chrono::high_resolution_clock::now();


	while (!window->windowShouldClose()) {
		tick();
		render();
		window->swapBuffers();
		window->pollEvents();

		fpsCounter++;

		auto now = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::milli>(now - start).count();
		if (elapsed >= 1000.0f) {
			window->setWindowTitle((std::string("Not advanced engine - FPS: ") + std::to_string(fpsCounter)).c_str());
			start = std::chrono::high_resolution_clock::now();
			fpsCounter = 0;
		}
	}
}

