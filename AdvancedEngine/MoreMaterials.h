#pragma once

#include "Material.h"

constexpr const char* transformVertexShaderSource = R"(
#version 460 core

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}
)";

constexpr const char* terrainVertexShaderSource = R"(
#version 460 core

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vNormal;

void main() {
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
	vNormal = aNormal;
}

)";

constexpr const char* terrainFragmentShaderSource = R"(
#version 460 core

out vec4 FragColor;

in vec3 vNormal;

const vec3 lightDirection = normalize(vec3(1.0, -1.0, 1.0));

void main() {

	float brightness = max(dot(normalize(vNormal), -lightDirection), 0.0);
	float clampedBrightness = mix(0.4, 1.0, brightness);


	FragColor = vec4(vec3(clampedBrightness), 1.0);
}
)";

class TransformMaterial : public Material {
public:
	TransformMaterial(const char* fragmentShaderSource) : Material(transformVertexShaderSource, fragmentShaderSource, {
		{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE } // position
		}) {
	};

};

class TerrainMaterial : public Material {
public:
	TerrainMaterial() : Material(terrainVertexShaderSource, terrainFragmentShaderSource, 
		
		{
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE }, // position
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE }  // normal
		}
	) 
	{
	};
};

