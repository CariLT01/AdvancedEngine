#pragma once

#include "Material.h"
#include "Texture.h"
#include <glad/gl.h>

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
out vec3 vPos;

void main() {
	vec4 worldPos = uModelMatrix * vec4(aPos, 1.0);
	gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
	vNormal = aNormal;
	vPos = worldPos.xyz;
}

)";



constexpr const char* terrainFragmentShaderSource = R"(
#version 460 core

uniform sampler2D uAlbedo;
uniform sampler2D uNormal;

out vec4 FragColor;

in vec3 vNormal;
in vec3 vPos;

const vec3 lightDirection = normalize(vec3(1.0, -1.0, 1.0));
const float scale = 1.0;

vec3 getTextureColor(sampler2D inTexture, vec3 vPos, vec3 vNormal) {
	vec3 absN = abs(vNormal);
	vec2 uvX = vPos.yz * scale;  // project onto YZ plane
	vec2 uvY = vPos.xz * scale;  // project onto XZ plane
	vec2 uvZ = vPos.xy * scale;  // project onto XY plane

	vec3 txX = texture(inTexture, uvX).rgb;
	vec3 txY = texture(inTexture, uvY).rgb;
	vec3 txZ = texture(inTexture, uvZ).rgb;

	vec3 color = (txX * absN.x + txY * absN.y + txZ * absN.z) / (absN.x + absN.y + absN.z);

	return color;
}

vec3 sampleNormalTriplanar(vec3 pos, vec3 normal)
{
    vec3 blend = abs(normal);
    blend = blend / (blend.x + blend.y + blend.z); // normalize

    // sample normal map along each plane
    vec3 nx = texture(uNormal, pos.yz).rgb * 2.0 - 1.0;
    vec3 ny = texture(uNormal, pos.xz).rgb * 2.0 - 1.0;
    vec3 nz = texture(uNormal, pos.xy).rgb * 2.0 - 1.0;

    // rotate to align with world axes
    nx = vec3(0.0, nx.x, nx.y);
    ny = vec3(ny.x, 0.0, ny.y);
    nz = vec3(nz.x, nz.y, 0.0);

    // blend normals
    vec3 n = normalize(nx * blend.x + ny * blend.y + nz * blend.z);
    return n;
}



void main() {



	// Calculate texture UV and color

	vec3 albedoColor = getTextureColor(uAlbedo, vPos, vNormal);

	// Normal map

	vec3 normal = sampleNormalTriplanar(vPos, vNormal);
	
	float brightness = max(dot(normalize(normal), -lightDirection), 0.0);
	float clampedBrightness = mix(0.4, 1.0, brightness);

	FragColor = vec4(albedoColor * vec3(clampedBrightness), 1.0);
}
)";

constexpr const char* gBufferTerrainVertexShaderSource = R"(
#version 460 core

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vNormal;
out vec3 vPos;

void main() {
	vec4 worldPos = uModelMatrix * vec4(aPos, 1.0);
	gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
	vNormal = aNormal;
	vPos = worldPos.xyz;
}

)";

constexpr const char* gBufferTerrainFragmentShaderSource = R"(
#version 460 core

uniform sampler2D uAlbedo;
uniform sampler2D uNormal;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gAlbedo;
layout (location = 2) out vec3 gNormal;

in vec3 vNormal;
in vec3 vPos;

const vec3 lightDirection = normalize(vec3(1.0, -1.0, 1.0));
const float scale = 1.0;

vec3 getTextureColor(sampler2D inTexture, vec3 vPos, vec3 vNormal) {
	vec3 absN = abs(vNormal);
	vec2 uvX = vPos.yz * scale;  // project onto YZ plane
	vec2 uvY = vPos.xz * scale;  // project onto XZ plane
	vec2 uvZ = vPos.xy * scale;  // project onto XY plane

	vec3 txX = texture(inTexture, uvX).rgb;
	vec3 txY = texture(inTexture, uvY).rgb;
	vec3 txZ = texture(inTexture, uvZ).rgb;

	vec3 color = (txX * absN.x + txY * absN.y + txZ * absN.z) / (absN.x + absN.y + absN.z);

	return color;
}

vec3 sampleNormalTriplanar(vec3 pos, vec3 normal)
{
    vec3 blend = abs(normal);
    blend = blend / (blend.x + blend.y + blend.z); // normalize

    // sample normal map along each plane
    vec3 nx = texture(uNormal, pos.yz).rgb * 2.0 - 1.0;
    vec3 ny = texture(uNormal, pos.xz).rgb * 2.0 - 1.0;
    vec3 nz = texture(uNormal, pos.xy).rgb * 2.0 - 1.0;

    // rotate to align with world axes
    nx = vec3(0.0, nx.x, nx.y);
    ny = vec3(ny.x, 0.0, ny.y);
    nz = vec3(nz.x, nz.y, 0.0);

    // blend normals
    vec3 n = normalize(nx * blend.x + ny * blend.y + nz * blend.z);
    return n;
}



void main() {



	// Calculate texture UV and color

	vec3 albedoColor = getTextureColor(uAlbedo, vPos, vNormal);

	// Normal map

	vec3 normal = sampleNormalTriplanar(vPos, vNormal);
	
	gPosition = vPos;
	gNormal = normal;
	gAlbedo = albedoColor;	


}
)";

class TransformMaterial : public Material {
public:
	TransformMaterial(const char* fragmentShaderSource) : Material(transformVertexShaderSource, fragmentShaderSource, {
		{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE } // position
		}) {
	};



};


class TerrainGBufferMaterial : public Material {
public:
	TerrainGBufferMaterial() : Material(gBufferTerrainVertexShaderSource, gBufferTerrainFragmentShaderSource,

		{
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE }, // position
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE }  // normal
		}
	), normalMapTexture(nullptr), albedoTexture(nullptr), roughnessTexture(nullptr), metallicTexture(nullptr)
	{
	};

	void use() override {
		shaderProgram->use();

		if (albedoTexture != nullptr) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedoTexture->textureID);
		}

		if (normalMapTexture != nullptr) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMapTexture->textureID);
		}

		int albedoTextureLoc = getUniformLocation("uAlbedo");
		int normalTextureLoc = getUniformLocation("uNormal");

		glUniform1i(albedoTextureLoc, 0);
		glUniform1i(normalTextureLoc, 1);
	}

	Texture* normalMapTexture;
	Texture* albedoTexture;
	Texture* roughnessTexture;
	Texture* metallicTexture;
};

constexpr const char* deferredShadingVertex = R"(
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vUv;

void main() {
	gl_Position = vec4(aPos.xy, 0.0, 1.0);
	vUv = aTexCoords;
}

)";

constexpr const char* deferredShadingFragment = R"(
#version 460 core

uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gPosition;

in vec2 vUv;

out vec4 FragColor;

const vec3 lightDirection = normalize(vec3(-1.0, -1.0, -1.0));

void main() {
	vec3 normal = texture(gNormal, vUv).rgb;

	float brightness = max(dot(normalize(normal), -lightDirection), 0.0);
	float clampedBrightness = mix(0.4, 1.0, brightness);

	vec3 albedoColor = texture(gAlbedo, vUv).rgb;

	FragColor = vec4(albedoColor * clampedBrightness, 1.0);
}


)";

class DeferredShadingMaterial : public Material {
public:
	DeferredShadingMaterial() : Material(deferredShadingVertex, deferredShadingFragment,

		{
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE }, // position
			{ sizeof(float) * 2, 2, GL_FLOAT, GL_FALSE }, // uv
		}
	)
	{

	}

	void use() override {
		shaderProgram->use();

		int gNormalLoc = getUniformLocation("gNormal");
		int gAlbedoLoc = getUniformLocation("gAlbedo");
		int gPositionLoc = getUniformLocation("gPosition");

		glUniform1i(gPositionLoc, 0);
		glUniform1i(gAlbedoLoc, 1);
		glUniform1i(gNormalLoc, 2);
	}

};