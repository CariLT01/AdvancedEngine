#pragma once

#include <glad/gl.h>

class Texture {
public:
	Texture(const char* path);

	void load();

	GLuint textureID;
};