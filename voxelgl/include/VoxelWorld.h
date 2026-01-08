#ifndef VOXEL_WORLD_H
#define VOXEL_WORLD_H

#include <glad/glad.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GreedyMesher.h"

class OpenCLHelper;
class GLShader;

struct Chunk {
	uint32_t id;
	glm::ivec3 position;  // En unidades de chunk
	int lodLevel;         // 0 = máximo detalle
	GLuint vao = 0;
	GLuint vbo = 0;
	int vertexCount = 0;
	bool needsUpdate = true;
	bool isVisible = true;
	std::vector<uint8_t> voxelData;  // 32x32x32 voxels
	float distanceToCamera = 0.0f;

	Chunk(glm::ivec3 pos, int lod = 0) : position(pos), lodLevel(lod) {
		id = (pos.x << 20) | (pos.y << 10) | pos.z;
		voxelData.resize(32 * 32 * 32, 0);
	}

	uint8_t getVoxel(int x, int y, int z) const {
		if (x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32)
			return 0;
		return voxelData[z * 32 * 32 + y * 32 + x];
	}

	void setVoxel(int x, int y, int z, uint8_t value) {
		if (x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32)
			return;
		voxelData[z * 32 * 32 + y * 32 + x] = value;
		needsUpdate = true;
	}
};

class VoxelWorld {
private:
	std::unordered_map<uint32_t, std::unique_ptr<Chunk>> chunks;
	std::unique_ptr<GreedyMesher> mesher;
	OpenCLHelper* clHelper = nullptr;

	int worldWidth, worldHeight, worldDepth;
	int chunkSize = 32;
	int renderDistance = 8;  // En chunks
	int maxLOD = 3;

	// Estadísticas
	int totalChunks = 0;
	int visibleChunks = 0;
	int renderedTriangles = 0;

	// Generación de terreno
	void generateChunkTerrain(Chunk* chunk);
	float noise3D(float x, float y, float z);

	// Gestión de chunks
	void updateChunkMesh(Chunk* chunk);
	void uploadChunkToGPU(Chunk* chunk, const std::vector<Vertex>& vertices);
	bool shouldUseLOD(const Chunk* chunk, const glm::vec3& cameraPos) const;
	int calculateLODLevel(const Chunk* chunk, const glm::vec3& cameraPos) const;

	// Culling
	bool isChunkInFrustum(const Chunk* chunk, const glm::mat4& viewProj) const;
	bool isChunkVisible(const Chunk* chunk, const glm::vec3& cameraPos) const;

public:
	VoxelWorld(int width, int height, int depth);
	~VoxelWorld();

	void setCLHelper(OpenCLHelper* helper) { clHelper = helper; }

	// Generación del mundo
	void generateTerrain();

	// Actualización
	void updateLOD(const glm::vec3& cameraPos);

	// Renderizado
	void render(GLShader* shader, const glm::vec3& cameraPos);

	// Utilidades
	Chunk* getOrCreateChunk(int cx, int cy, int cz);
	uint8_t getWorldVoxel(int wx, int wy, int wz);

	// Estadísticas
	int getTotalChunks() const { return totalChunks; }
	int getVisibleChunks() const { return visibleChunks; }
	int getRenderedTriangles() const { return renderedTriangles; }
};

#endif