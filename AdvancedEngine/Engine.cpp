#include "Engine.h"

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

void Engine::initialize() {
	// Initialization code can go here if needed

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set a clear color
	glViewport(0, 0, currentWidth, currentHeight);

	initializeDebuggingObjects();
}

void Engine::initializeDebuggingObjects() {

	VertexAttribute positionVA = VertexAttribute{
		.sizeInBytes = sizeof(float) * 3,
		.size = 3,
		.type = GL_FLOAT,
		.normalized = GL_FALSE
	};

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



	material = new Material(vertexShaderSource, fragmentShaderSource, { positionVA });
	mesh = new Mesh(vertices, indices, material);

}

void Engine::tick() {

}

void Engine::render() {

	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mesh->render();
}

void Engine::run() {
	while (!window->windowShouldClose()) {
		tick();
		render();
		window->swapBuffers();
		window->pollEvents();
	}
}

