#include "Material.h"

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