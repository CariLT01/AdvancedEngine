#include "Material.h"
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

Material::Material(const char* vertexShaderSource, const char* fragmentShaderSource, std::vector<VertexAttribute> vertexAttributes) {
	if (vertexAttributes.empty()) {
		throw std::runtime_error("Vertex attributes cannot be empty");
	}

	shaderProgram = new ShaderProgram();
	Shader vertexShader(vertexShaderSource, GL_VERTEX_SHADER);
	vertexShader.compile();
	shaderProgram->attachShader(vertexShader);
	Shader fragmentShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
	fragmentShader.compile();
	shaderProgram->attachShader(fragmentShader);
	shaderProgram->link();
	

	this->vertexAttributes = vertexAttributes;
}

void Material::use() {
	shaderProgram->use();
}
void Material::setMatrices(Camera* camera) {
	int viewMatrixLoc = getUniformLocation("uViewMatrix");
	int projectionMatrixLoc = getUniformLocation("uProjectionMatrix");

	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(camera->viewMatrix));
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(camera->projectionMatrix));
}

int Material::getUniformLocation(const char* name) {
	return shaderProgram->getUniformLocation(name);
}	

void Material::use2(Camera* camera) {

}