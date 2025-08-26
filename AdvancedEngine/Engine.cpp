#include "Engine.h"
#include "MoreMaterials.h"
#include "MarchingCubesGenerator.h"
#include "Settings.h"
#include <FastNoise/FastNoise.h>
#include <chrono>
#include <cstdlib>
#include <ctime>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
const float MOVEMENT_SPEED = 5.0f;
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

	const float SENSITIVITY = 0.1f;

	if (window->getKeyPressed(GLFW_KEY_W) == GLFW_PRESS) {
		camera->position += camera->direction * deltaTime * MOVEMENT_SPEED;
		camera->recomputeMatrices();
	}
	if (window->getKeyPressed(GLFW_KEY_A) == GLFW_PRESS) {
		camera->position += -camera->cameraRight * deltaTime * MOVEMENT_SPEED;
		camera->recomputeMatrices();
	}
	if (window->getKeyPressed(GLFW_KEY_S) == GLFW_PRESS) {
		camera->position += camera->direction * -deltaTime * MOVEMENT_SPEED;
		camera->recomputeMatrices();
	}
	if (window->getKeyPressed(GLFW_KEY_D) == GLFW_PRESS) {
		camera->position += camera->cameraRight * deltaTime * MOVEMENT_SPEED;
		camera->recomputeMatrices();
	}

	double posX, posY;

	glfwGetCursorPos(window->window, &posX, &posY);

	double deltaX = posX - lastX;
	double deltaY = posY - lastY;

	lastX = posX;
	lastY = posY;

	camera->yaw += deltaX * SENSITIVITY;
	camera->pitch += -deltaY * SENSITIVITY;

	camera->yaw = fmod(camera->yaw, 360.0f);
	camera->pitch = glm::clamp(camera->pitch, -89.0f, 89.0f);
	camera->recomputeMatrices();


	
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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	initializeCamera();
	initializeWorld();
	//initializeDebuggingObjects();
	
	registerEvents();
}

void Engine::initializeWorld() {
	chunksManager = new ChunksManager(camera);
}

void Engine::initializeCamera() {

	camera = new Camera(glm::vec3(0, 0, 5), 90.f);
	camera->aspectRatio = static_cast<float>(window->windowWidth) / static_cast<float>(window->windowHeight);

	camera->recomputeMatrices();

}

void Engine::registerEvents() {
	window->onResize = std::bind(&Engine::onWindowResize, this);
}

/*void Engine::initializeDebuggingObjects() {

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

	//material = new TransformMaterial(fragmentShaderSource);
	//Mesh* mesh = new Mesh(vertices, indices, material);

	
	//worldObject = new WorldObject(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), mesh, camera);

	// --- Generate a grid of random values ---

	std::vector<float> densities;
	//densities.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	std::vector<unsigned int> materials = {};

	auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
	
	const float scale = 0.03f;

	srand(time(nullptr));

	std::vector<float> heightMap(CHUNK_SIZE * CHUNK_SIZE);


	fnSimplex->GenUniformGrid2D(heightMap.data(), 0, 0, CHUNK_SIZE, CHUNK_SIZE, 0.06, rand());

	for (unsigned int y = 0; y < CHUNK_SIZE; y++) {
		for (unsigned int x = 0; x < CHUNK_SIZE; x++) {
			for (unsigned int z = 0; z < CHUNK_SIZE; z++) {


				//const float v = fnSimplex->GenSingle3D((float)x * scale, (float)y * scale, (float)z * scale, 68);
				//const float v = rand() / (float)RAND_MAX * 2.0f - 1.0f;
				//densities.push_back((v + 1) / 2.0f);
				const float heightMapValue = (heightMap[x * CHUNK_SIZE + z] + 1) / 2.0f;
				const float surfaceHeight = heightMapValue * 10.0f;

				densities.push_back(std::clamp(surfaceHeight - y, 0.0f, 1.0f));

				//densities[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = (v + 1) / 2.0f;
				materials.push_back(1);

			}
		}
	}

	MarchingCubeGenerator generator(0.5f);

	const std::vector<float> vertices = generator.generateMesh(densities, materials, 1);

	std::vector<unsigned int> indices = {};

	for (unsigned int i = 0; i < vertices.size() / 6; i++) {
		indices.push_back(i);
	}

	material = new TerrainMaterial();

	Mesh* mesh = new Mesh(vertices, indices, material);
	worldObject = new WorldObject(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), mesh, camera);
}*/

void Engine::tick() {
	handleCameraInput();

	// Get the current position in chunk coordinates

	const glm::vec3& currentCameraPosition = camera->position;
	const glm::vec3 currentChunkPosition = glm::vec3(
		std::floor(camera->position.x / CHUNK_SIZE),
		std::floor(camera->position.y / CHUNK_SIZE),
		std::floor(camera->position.z / CHUNK_SIZE)
	);

	chunksManager->tick(currentChunkPosition);
}

void Engine::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	chunksManager->renderChunks();
}

void Engine::run() {

	int fpsCounter = 0;
	auto start = std::chrono::high_resolution_clock::now();


	while (!window->windowShouldClose()) {

		auto deltaTimeStart = std::chrono::high_resolution_clock::now();

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

		auto deltaTimeEnd = std::chrono::high_resolution_clock::now();

		deltaTime = std::chrono::duration<float, std::milli>(deltaTimeEnd - deltaTimeStart).count() / 1000.0f;
	}
}

