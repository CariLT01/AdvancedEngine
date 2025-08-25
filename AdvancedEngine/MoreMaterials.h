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


class TransformMaterial : public Material {
public:
	TransformMaterial(const char* fragmentShaderSource) : Material(transformVertexShaderSource, fragmentShaderSource, {
		{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE } // position
		}) {
	};

};