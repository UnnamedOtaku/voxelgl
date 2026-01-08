#include "GreedyMesher.h"
#include <cstring>
#include <algorithm>
#include <iostream>

GreedyMesher::GreedyMesher() {
	visitedBuffer = nullptr;
	bufferSize = 0;
}

GreedyMesher::~GreedyMesher() {
	delete[] visitedBuffer;
}

void GreedyMesher::ensureVisitedBuffer(int size) {
	if (size > bufferSize) {
		delete[] visitedBuffer;
		visitedBuffer = new bool[size];
		bufferSize = size;
	}
}

bool GreedyMesher::isSolid(const uint8_t* voxels, const glm::ivec3& size, const glm::ivec3& pos) {
	if (pos.x < 0 || pos.x >= size.x ||
		pos.y < 0 || pos.y >= size.y ||
		pos.z < 0 || pos.z >= size.z) {
		return false;
	}

	int idx = pos.z * size.y * size.x + pos.y * size.x + pos.x;
	return voxels[idx] != 0;
}

bool GreedyMesher::isFaceVisible(const uint8_t* voxels, const glm::ivec3& size,
	const glm::ivec3& pos, int face) {
	glm::ivec3 neighbor = pos;

	switch (face) {
	case 0: neighbor.x += 1; break; // +X
	case 1: neighbor.x -= 1; break; // -X
	case 2: neighbor.y += 1; break; // +Y
	case 3: neighbor.y -= 1; break; // -Y
	case 4: neighbor.z += 1; break; // +Z
	case 5: neighbor.z -= 1; break; // -Z
	}

	return !isSolid(voxels, size, neighbor);
}

std::vector<Cuboid> GreedyMesher::greedy3DBinary(const uint8_t* voxels, const glm::ivec3& size) {
	std::vector<Cuboid> cuboids;
	int totalVoxels = size.x * size.y * size.z;

	ensureVisitedBuffer(totalVoxels);
	memset(visitedBuffer, 0, totalVoxels * sizeof(bool));

	for (int z = 0; z < size.z; z++) {
		for (int y = 0; y < size.y; y++) {
			for (int x = 0; x < size.x; x++) {
				glm::ivec3 pos(x, y, z);
				int idx = z * size.y * size.x + y * size.x + x;

				// Saltar vacío o ya visitado
				if (visitedBuffer[idx] || voxels[idx] == 0) continue;

				uint8_t material = voxels[idx];
				Cuboid cuboid;
				cuboid.min = pos;
				cuboid.max = pos;
				cuboid.material = material;

				// Expandir en X
				for (int dx = 1; x + dx < size.x; dx++) {
					glm::ivec3 checkPos = pos + glm::ivec3(dx, 0, 0);
					int checkIdx = checkPos.z * size.y * size.x + checkPos.y * size.x + checkPos.x;

					if (visitedBuffer[checkIdx] || voxels[checkIdx] != material) break;
					cuboid.max.x = x + dx;
				}

				// Expandir en Y
				bool canExpandY = true;
				for (int dy = 1; y + dy < size.y && canExpandY; dy++) {
					for (int dx = 0; dx <= cuboid.max.x - cuboid.min.x; dx++) {
						glm::ivec3 checkPos = cuboid.min + glm::ivec3(dx, dy, 0);
						int checkIdx = checkPos.z * size.y * size.x + checkPos.y * size.x + checkPos.x;

						if (visitedBuffer[checkIdx] || voxels[checkIdx] != material) {
							canExpandY = false;
							break;
						}
					}
					if (canExpandY) cuboid.max.y = y + dy;
				}

				// Expandir en Z
				bool canExpandZ = true;
				for (int dz = 1; z + dz < size.z && canExpandZ; dz++) {
					for (int dy = 0; dy <= cuboid.max.y - cuboid.min.y; dy++) {
						for (int dx = 0; dx <= cuboid.max.x - cuboid.min.x; dx++) {
							glm::ivec3 checkPos = cuboid.min + glm::ivec3(dx, dy, dz);
							int checkIdx = checkPos.z * size.y * size.x + checkPos.y * size.x + checkPos.x;

							if (visitedBuffer[checkIdx] || voxels[checkIdx] != material) {
								canExpandZ = false;
								break;
							}
						}
						if (!canExpandZ) break;
					}
					if (canExpandZ) cuboid.max.z = z + dz;
				}

				// Marcar como visitado
				for (int dz = cuboid.min.z; dz <= cuboid.max.z; dz++) {
					for (int dy = cuboid.min.y; dy <= cuboid.max.y; dy++) {
						for (int dx = cuboid.min.x; dx <= cuboid.max.x; dx++) {
							int vIdx = dz * size.y * size.x + dy * size.x + dx;
							visitedBuffer[vIdx] = true;
						}
					}
				}

				cuboids.push_back(cuboid);
			}
		}
	}

	return cuboids;
}

Mesh GreedyMesher::cuboidsToVertices(const std::vector<Cuboid>& cuboids) {
	Mesh mesh;
	int id = 0;

	for (const auto& cuboid : cuboids) {
		glm::vec3 minPos = glm::vec3(cuboid.min) - 0.5f;
		glm::vec3 maxPos = glm::vec3(cuboid.max) + 0.5f;
		uint32_t material = cuboid.material;

		// Definir los 8 vértices del cuboide
		glm::vec3 vertices3D[8] = {
			glm::vec3(minPos.x, minPos.y, minPos.z),
			glm::vec3(maxPos.x, minPos.y, minPos.z),
			glm::vec3(maxPos.x, maxPos.y, minPos.z),
			glm::vec3(minPos.x, maxPos.y, minPos.z),
			glm::vec3(minPos.x, minPos.y, maxPos.z),
			glm::vec3(maxPos.x, minPos.y, maxPos.z),
			glm::vec3(maxPos.x, maxPos.y, maxPos.z),
			glm::vec3(minPos.x, maxPos.y, maxPos.z)
		};

		// Caras del cubo (6 caras, 2 triángulos cada una)
		int faceIndices[6][4] = {
			{ 1, 2, 6, 5 }, // +X
			{ 0, 4, 7, 3 }, // -X
			{ 3, 7, 6, 2 }, // +Y
			{ 0, 1, 5, 4 }, // -Y
			{ 4, 5, 6, 7 }, // +Z
			{ 0, 3, 2, 1 }  // -Z
		};

		int voxelIndices[6][4] = {
			{0, 1, 2, 3},       // Front
			{4, 5, 6, 7},       // Back
			{8, 9, 10, 11},     // Right
			{12, 13, 14, 15},   // Left
			{16, 17, 18, 19},   // Top
			{20, 21, 22, 23}    // Bottom
		};

		glm::vec2 uv[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		for (int face = 0; face < 6; face++) {
			// Solo agregar caras visibles (en este caso, todas)
			glm::vec3 normal = faceNormals[face];

			// Crear dos triángulos para la cara
			mesh.vertices.emplace_back(vertices3D[faceIndices[face][0]], normal, uv[0], material);
			mesh.vertices.emplace_back(vertices3D[faceIndices[face][1]], normal, uv[1], material);
			mesh.vertices.emplace_back(vertices3D[faceIndices[face][2]], normal, uv[2], material);
			mesh.vertices.emplace_back(vertices3D[faceIndices[face][3]], normal, uv[3], material);

			// Triángulo 1
			mesh.indices.push_back(voxelIndices[face][0] + id * 24);
			mesh.indices.push_back(voxelIndices[face][1] + id * 24);
			mesh.indices.push_back(voxelIndices[face][2] + id * 24);

			// Triángulo 2
			mesh.indices.push_back(voxelIndices[face][0] + id * 24);
			mesh.indices.push_back(voxelIndices[face][2] + id * 24);
			mesh.indices.push_back(voxelIndices[face][3] + id * 24);
		}
		id++;
	}

	return mesh;
}

Mesh GreedyMesher::greedy3DBinaryToVertices(const uint8_t* voxels, const glm::ivec3& size) {
	auto cuboids = greedy3DBinary(voxels, size);
	return cuboidsToVertices(cuboids);
}

std::vector<uint8_t> GreedyMesher::downsample(const uint8_t* voxels,
	const glm::ivec3& size, int factor) {
	glm::ivec3 newSize = size / factor;
	std::vector<uint8_t> result(newSize.x * newSize.y * newSize.z, 0);

	for (int z = 0; z < newSize.z; z++) {
		for (int y = 0; y < newSize.y; y++) {
			for (int x = 0; x < newSize.x; x++) {
				int solidCount = 0;
				int totalCount = 0;

				for (int dz = 0; dz < factor; dz++) {
					for (int dy = 0; dy < factor; dy++) {
						for (int dx = 0; dx < factor; dx++) {
							int sx = x * factor + dx;
							int sy = y * factor + dy;
							int sz = z * factor + dz;

							if (sx < size.x && sy < size.y && sz < size.z) {
								int idx = sz * size.y * size.x + sy * size.x + sx;
								if (voxels[idx] > 0) solidCount++;
								totalCount++;
							}
						}
					}
				}

				// Si más del 50% son sólidos, hacer voxel sólido
				int idx = z * newSize.y * newSize.x + y * newSize.x + x;
				if (totalCount > 0 && solidCount * 2 > totalCount) {
					result[idx] = 1;  // Material base para LOD
				}
			}
		}
	}

	return result;
}

Mesh GreedyMesher::generateLODMesh(const uint8_t* voxels,
	const glm::ivec3& size, int lodLevel) {
	if (lodLevel == 0) {
		return greedy3DBinaryToVertices(voxels, size);
	}

	int factor = 1 << lodLevel;
	auto downsampled = downsample(voxels, size, factor);
	glm::ivec3 newSize = size / factor;

	auto cuboids = greedy3DBinary(downsampled.data(), newSize);

	// Escalar cuboides de vuelta
	for (auto& cuboid : cuboids) {
		cuboid.min *= factor;
		cuboid.max = (cuboid.max + glm::ivec3(1)) * factor - glm::ivec3(1);
	}

	return cuboidsToVertices(cuboids);
}