#include "Chunk.h"
#include "Material.h"
#include "Settings.h"
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

Chunk::Chunk(const glm::vec3& chunkPosition, const std::vector<float>& densities, const std::vector<unsigned int>& materials) : chunkPosition(chunkPosition), densities(densities), materials(materials) {

}

Chunk::~Chunk() {
	if (chunkObject == nullptr) return;
	if (chunkMesh == nullptr) return;
	delete chunkObject;
	delete chunkMesh;
}

void Chunk::buildChunk(Material* material, MarchingCubeGenerator* generator, Camera* camera, PhysicsEngine* physicsEngine) {
    std::vector<float> vertices = generator->generateMesh(densities, materials, 1);
    if (vertices.size() == 0) return;

    // Create graphics mesh (unchanged)
    std::vector<unsigned int> indices(vertices.size() / 6);
    for (unsigned int i = 0; i < vertices.size() / 6; i++) {
        indices[i] = i;
    }
    chunkMesh = new Mesh(vertices, indices, material);
    chunkObject = new WorldObject(chunkPosition * static_cast<float>(CHUNK_SIZE), glm::vec3(0), glm::vec3(1), chunkMesh, camera);

    // Prepare physics mesh data
    JPH::VertexList verticesList;
    JPH::IndexedTriangleList triangles;

    // Each triangle has 3 vertices, each vertex has 6 floats
    unsigned int numTriangles = vertices.size() / (6 * 3);
    verticesList.reserve(numTriangles * 3);
    triangles.reserve(numTriangles);

    for (unsigned int tri = 0; tri < numTriangles; tri++) {
        unsigned int baseIndex = tri * 3 * 6; // Skip 6 floats per vertex, 3 vertices per triangle

        // Add vertices for this triangle
        for (int i = 0; i < 3; i++) {
            unsigned int offset = baseIndex + i * 6;
            verticesList.push_back(JPH::Float3(
                vertices[offset],     // x
                vertices[offset + 1], // y
                vertices[offset + 2]  // z
            ));
        }

        // Add triangle (vertices are added in order, so indices are sequential)
        unsigned int firstVertexIndex = tri * 3;
        triangles.push_back(JPH::IndexedTriangle(
            firstVertexIndex,
            firstVertexIndex + 1,
            firstVertexIndex + 2
        ));
    }

    JPH::MeshShapeSettings meshSettings(verticesList, triangles);
    

    JPH::ShapeSettings::ShapeResult shapeResult = meshSettings.Create();
    if (shapeResult.IsValid()) {
        chunkShape = shapeResult.Get();
        JPH::BodyCreationSettings bcs(
            chunkShape,
            JPH::Vec3(chunkPosition.x * CHUNK_SIZE, chunkPosition.y * CHUNK_SIZE, chunkPosition.z * CHUNK_SIZE),
            JPH::Quat::sIdentity(),
            JPH::EMotionType::Static,
            Layers::NON_MOVING
        );
        chunkBody = physicsEngine->bodyInterface->CreateBody(bcs);
    }
}


void Chunk::render() {
	if (chunkObject != nullptr && chunkMesh != nullptr) {
		chunkObject->render();
	}
	
}

