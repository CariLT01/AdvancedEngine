#pragma once

#include <glad/gl.h>
#include <vector>

class Texture {
public:
	Texture(std::vector<const char*> paths);

	void load();

	GLuint textureID;
};