#pragma once

#include "Material.h"
#include "Texture.h"
#include <glad/gl.h>
#include "Settings.h"

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
layout (location = 2) in float aMaterial;

out vec3 vNormal;
out vec3 vPos;
flat out uint vMaterial;

void main() {
	vec4 worldPos = uModelMatrix * vec4(aPos, 1.0);
	gl_Position = uProjectionMatrix * uViewMatrix * worldPos;
	vNormal = aNormal;
	vPos = worldPos.xyz;
	vMaterial = uint(aMaterial);
}

)";

constexpr const char* gBufferTerrainFragmentShaderSource = R"(
#version 460 core

uniform int uAtlasWidth;
uniform int uAtlasHeight;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec2 gSkyMaterial;

in vec3 vNormal;
in vec3 vPos;
flat in uint vMaterial;


void main() {
	
	gPosition = vPos;
	gNormal = (vNormal * 0.5) + vec3(0.5);
    gSkyMaterial = vec2(1.0, float(vMaterial) / 255.0);

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
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE },  // normal
			{ sizeof(float) * 1, 1, GL_FLOAT, GL_FALSE}, // Material. Should actually be unsigned int, but oh well.
		}
	)
	{
	};

	void use() override {
		shaderProgram->use();



	}

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
uniform sampler2D gPosition;
uniform sampler2D gSkyMaterial;
uniform vec3 uCameraPosition;

uniform sampler2D uAlbedo;
uniform sampler2D uNormal;
uniform sampler2D uRoughness;
uniform sampler2D uMetallic;
uniform sampler2D uAo;


in vec2 vUv;

out vec4 FragColor;

const float PI = 3.14159265358979323846264338327950288;

const float scale = 0.5;
const int uNumTextures = 3;


vec3 getTextureColor(sampler2D inTexture, vec3 vPos, vec3 vNormal, float vMaterial) {
    vec3 absN = abs(vNormal);
    
    // Calculate the height of each texture in the atlas (normalized)
    float textureHeight = 1.0 / float(uNumTextures);
    float yOffset = vMaterial * textureHeight;
    
    // Project onto each plane and apply scale
    vec2 uvX = vPos.yz * scale;
    vec2 uvY = vPos.xz * scale;
    vec2 uvZ = vPos.xy * scale;
    
    // Wrap UV coordinates to [0,1] range for repetition
    uvX = fract(uvX);
    uvY = fract(uvY);
    uvZ = fract(uvZ);
    
    // Adjust Y coordinates to sample from the correct texture in the atlas
    uvX.y = uvX.y * textureHeight + yOffset;
    uvY.y = uvY.y * textureHeight + yOffset;
    uvZ.y = uvZ.y * textureHeight + yOffset;
    
    // Sample from the texture atlas
    vec3 txX = texture(inTexture, uvX).rgb;
    vec3 txY = texture(inTexture, uvY).rgb;
    vec3 txZ = texture(inTexture, uvZ).rgb;
    
    // Blend the samples based on the normal
    vec3 color = (txX * absN.x + txY * absN.y + txZ * absN.z) / (absN.x + absN.y + absN.z);
    
    return color;
}

float getTextureColorR(sampler2D inTexture, vec3 vPos, vec3 vNormal, float vMaterial) {
    vec3 absN = abs(vNormal);
    
    // Calculate the height of each texture in the atlas (normalized)
    float textureHeight = 1.0 / uNumTextures;
    float yOffset = vMaterial * textureHeight;
    
    // Project onto each plane and apply scale
    vec2 uvX = vPos.yz * scale;
    vec2 uvY = vPos.xz * scale;
    vec2 uvZ = vPos.xy * scale;
    
    // Wrap UV coordinates to [0,1] range for repetition
    uvX = fract(uvX);
    uvY = fract(uvY);
    uvZ = fract(uvZ);
    
    // Adjust Y coordinates to sample from the correct texture in the atlas
    uvX.y = uvX.y * textureHeight + yOffset;
    uvY.y = uvY.y * textureHeight + yOffset;
    uvZ.y = uvZ.y * textureHeight + yOffset;
    
    // Sample from the texture atlas
    float txX = texture(inTexture, uvX).r;
    float txY = texture(inTexture, uvY).r;
    float txZ = texture(inTexture, uvZ).r;
    
    // Blend the samples based on the normal
    float color = (txX * absN.x + txY * absN.y + txZ * absN.z) / (absN.x + absN.y + absN.z);
    
    return color;
}

vec3 sampleNormalTriplanar(vec3 pos, vec3 geomNormal, float vMaterial) {
    // 1. Calculate Blend Weights (Your original code for this is good)
    // We use the absolute value of the geometry normal to determine the influence of each projection.
    vec3 blendWeights = abs(geomNormal);
    // This normalization technique is robust and prevents division by zero.
    blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

    // 2. Atlas/UV calculations (Same as your other functions)
    float textureHeight = 1.0 / float(uNumTextures);
    float yOffset = float(vMaterial) * textureHeight;

    vec2 uvX = pos.yz * scale;
    vec2 uvY = pos.xz * scale;
    vec2 uvZ = pos.xy * scale;

    uvX = fract(uvX);
    uvY = fract(uvY);
    uvZ = fract(uvZ);

    uvX.y = uvX.y * textureHeight + yOffset;
    uvY.y = uvY.y * textureHeight + yOffset;
    uvZ.y = uvZ.y * textureHeight + yOffset;

    // 3. Sample normal maps and unpack from [0, 1] to [-1, 1] range
    vec3 sx = texture(uNormal, uvX).rgb * 2.0 - 1.0;
    vec3 sy = texture(uNormal, uvY).rgb * 2.0 - 1.0;
    vec3 sz = texture(uNormal, uvZ).rgb * 2.0 - 1.0;

    // 4. Swizzle sampled normals to orient them correctly in world space
    // For a tangent-space normal (T, B, N), we remap it to world-space (X, Y, Z)
    // Projection from X (using YZ for UVs): Tangent->Y, Bitangent->Z, Normal->X
    vec3 nX = vec3(sx.z, sx.x, sx.y); 
    // Projection from Y (using XZ for UVs): Tangent->X, Bitangent->Z, Normal->Y
    vec3 nY = vec3(sy.x, sy.z, sy.y);
    // Projection from Z (using XY for UVs): Tangent->X, Bitangent->Y, Normal->Z
    vec3 nZ = vec3(sz.x, sz.y, sz.z);

    // Optional: Account for the direction of the geometry normal.
    // This fixes lighting on faces pointing in negative directions (e.g., the bottom or back of a cube).
    nX *= sign(geomNormal.x);
    nY *= sign(geomNormal.y);
    nZ *= sign(geomNormal.z);
    
    // 5. Blend the world-space normals using the weights and normalize
    // The geometry normal (vNormal) plays its crucial role here in the blending.
    vec3 finalNormal = nX * blendWeights.x + nY * blendWeights.y + nZ * blendWeights.z;

    return normalize(finalNormal);
}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 PBRLighting(vec3 N, vec3 V, vec3 L,
                 vec3 albedo, float metallic, float roughness,
                 vec3 lightColor)
{
 
    vec3 H = normalize(V + L);  

    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
 
    float NDF = DistributionGGX(N, H, roughness);       
    float G   = GeometrySmith(N, V, L, roughness);  

    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
    vec3 specular     = numerator / denominator;  


    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
  
    kD *= 1.0 - metallic;	

    
    const float PI = 3.14159265359;
  
    float NdotL = max(dot(N, L), 0.0);        
    return (kD * albedo / PI + specular) * lightColor * NdotL; // Replace lightColor with radiance at learnopengl.com

}

const vec3 lightDirection = normalize(vec3(-1.0, -1.0, -1.0));
const vec3 lightColor = vec3(4.0);
const float CULLING_DISTANCE_SQUARED = 150.0 * 150.0;

void main() {

    vec2 skyMaterial = texture(gSkyMaterial, vUv).rg;
    if (skyMaterial.r < 1.0) discard;
    float material = float(skyMaterial.y * 255.0);

    vec3 fragPosition = texture(gPosition, vUv).rgb;

    float distanceSquared = dot(fragPosition - uCameraPosition, fragPosition - uCameraPosition);

    vec3 vNormal = ((texture(gNormal, vUv).rgb) * 2) - vec3(1.0);
    vec3 albedo = pow(getTextureColor(uAlbedo, fragPosition, vNormal, material), vec3(2.2));

    if (distanceSquared < CULLING_DISTANCE_SQUARED) {
            
            vec3 normal = sampleNormalTriplanar(fragPosition, vNormal, material);    

            
            float roughness = getTextureColorR(uRoughness, fragPosition, vNormal, material);
            float metallic = getTextureColorR(uMetallic, fragPosition, vNormal, material);
            float ao = getTextureColorR(uAo, fragPosition, vNormal, material);

            ///// Physically based rendering /////
    

            vec3 V = normalize(uCameraPosition - fragPosition);
            vec3 L = normalize(-lightDirection);
    
            vec3 Lo =  PBRLighting(normal, V, L, albedo, metallic, roughness, lightColor);

	
            vec3 ambient = vec3(0.03) * albedo * ao;
            vec3 finalColor = ambient + Lo;




	        //FragColor = vec4(albedoColor * clampedBrightness, 1.0);
            //FragColor = vec4(roughness, roughness, roughness, 1.0);
            //FragColor = vec4(vec3(metallic), 1.0);

            //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
            FragColor = vec4(pow(finalColor, vec3(1.0 / 2.2)), 1.0);


            //FragColor = vec4(normal, 1.0);
            //FragColor = vec4(fragPosition, 1.0);
            //FragColor = vec4(gRMA, 1.0);
    } else {
        FragColor = vec4(pow(albedo, vec3(1.0 / 2.2)), 1.0);
    }


}


)";

class DeferredShadingMaterial : public Material {
public:
	DeferredShadingMaterial() : Material(deferredShadingVertex, deferredShadingFragment,

		{
			{ sizeof(float) * 3, 3, GL_FLOAT, GL_FALSE }, // position
			{ sizeof(float) * 2, 2, GL_FLOAT, GL_FALSE }, // uv
		}
	), normalMapTexture(nullptr), albedoTexture(nullptr), roughnessTexture(nullptr), metallicTexture(nullptr)
	{

	}

	void use() override {


	}

    void use2(Camera* camera) override {
        shaderProgram->use();

        int gNormalLoc = getUniformLocation("gNormal");
        //int gAlbedoLoc = getUniformLocation("gAlbedo");
        int gPositionLoc = getUniformLocation("gPosition");
       // int gRoughnessMetallicAo = getUniformLocation("gRoughnessMetallicAo");
        int uCameraPositionLoc = getUniformLocation("uCameraPosition");
        int gSkyMaterialLoc = getUniformLocation("gSkyMaterial");

        glUniform1i(gPositionLoc, 0);
       // glUniform1i(gAlbedoLoc, 1);
        glUniform1i(gNormalLoc, 1);
        //glUniform1i(gRoughnessMetallicAo, 3);
        glUniform3f(uCameraPositionLoc, camera->position.x, camera->position.y, camera->position.z);
        glUniform1i(gSkyMaterialLoc, 2);

        if (albedoTexture != nullptr) {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, albedoTexture->textureID);
        }

        if (normalMapTexture != nullptr) {
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, normalMapTexture->textureID);
        }

        if (roughnessTexture != nullptr) {
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, roughnessTexture->textureID);
        }

        if (metallicTexture != nullptr) {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, metallicTexture->textureID);
        }
        if (aoTexture != nullptr) {
            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_2D, aoTexture->textureID);
        }

        int albedoTextureLoc = getUniformLocation("uAlbedo");
        int normalTextureLoc = getUniformLocation("uNormal");
        int roughnessTextureLoc = getUniformLocation("uRoughness");
        int metallicTextureLoc = getUniformLocation("uMetallic");
        int aoTextureLoc = getUniformLocation("uAo");

        glUniform1i(albedoTextureLoc, 4);
        glUniform1i(normalTextureLoc, 5);
        glUniform1i(roughnessTextureLoc, 6);
        glUniform1i(metallicTextureLoc, 7);
        glUniform1i(aoTextureLoc, 8);

        int uWidthLoc = getUniformLocation("uWidth");
        int uHeightLoc = getUniformLocation("uHeight");

        glUniform1i(uWidthLoc, PBR_SIZE);
        glUniform1i(uHeightLoc, PBR_SIZE * N_MATERIALS);
    }

    Texture* normalMapTexture;
    Texture* albedoTexture;
    Texture* roughnessTexture;
    Texture* metallicTexture;
    Texture* aoTexture;

};