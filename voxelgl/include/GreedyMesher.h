#ifndef GREEDY_MESHER_H
#define GREEDY_MESHER_H

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	uint32_t material;

	Vertex() = default;
	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex, uint32_t mat)
		: position(pos), normal(norm), uv(tex), material(mat) {}
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct Cuboid {
	glm::ivec3 min;
	glm::ivec3 max;
	uint32_t material;
};

class GreedyMesher {
private:
	// Buffer para marcado de visitados
	bool* visitedBuffer = nullptr;
	int bufferSize = 0;

	// Direcciones normales
	const glm::vec3 faceNormals[6] = {
		glm::vec3(1, 0, 0),   // +X
		glm::vec3(-1, 0, 0),  // -X
		glm::vec3(0, 1, 0),   // +Y
		glm::vec3(0, -1, 0),  // -Y
		glm::vec3(0, 0, 1),   // +Z
		glm::vec3(0, 0, -1)   // -Z
	};

public:
	GreedyMesher();
	~GreedyMesher();

	// Greedy meshing 3D binario (sólido/vacío)
	std::vector<Cuboid> greedy3DBinary(const uint8_t* voxels, const glm::ivec3& size);

	// Convertir cuboides a vértices
	Mesh cuboidsToVertices(const std::vector<Cuboid>& cuboids);

	// Función combinada para fácil uso
	Mesh greedy3DBinaryToVertices(const uint8_t* voxels, const glm::ivec3& size);

	// LOD: Downsample y greedy meshing
	Mesh generateLODMesh(const uint8_t* voxels, const glm::ivec3& size, int lodLevel);

private:
	// Funciones auxiliares
	void ensureVisitedBuffer(int size);
	bool isSolid(const uint8_t* voxels, const glm::ivec3& size, const glm::ivec3& pos);
	bool isFaceVisible(const uint8_t* voxels, const glm::ivec3& size,
		const glm::ivec3& pos, int face);

	// Downsampling para LOD
	std::vector<uint8_t> downsample(const uint8_t* voxels,
		const glm::ivec3& size, int factor);
};

#endif