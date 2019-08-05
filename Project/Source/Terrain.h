#pragma once

#include <glm/glm.hpp>

class PerlinNoise;

class Terrain {
public:


private:
	class Chunk {
	public:
		Chunk();
		Chunk(int xCoord, int yCoord);
		~Chunk();

		void GenVertexBuffer();
		void Draw();

		static PerlinNoise noiseGenerator;
		const static int CHUNK_SIZE = 512;

		unsigned int mVAO;
		unsigned int mVBO;

		int mXCoord;
		int mYCoord;
		float mHeightMap[CHUNK_SIZE][CHUNK_SIZE];
	};

public:
	Terrain();
	Terrain(unsigned int width, unsigned int height);
	~Terrain();

	void Draw();
	void Erode();

	//To get height at a point between nodes, we will find which triangle the point resides in and then interpolate the heights of the triangle vertices to find the hieght at the given point
	float GetHeight(float xCoord, float yCoord);

	const unsigned int mWidth;
	const unsigned int mHeight;
	Chunk*** mChunkMap;

	//To get the height at a node, we will have to find the chunk that said node belongs to and the position of the node within that chunk
	float& GetHeightRef(int xCoord, int yCoord)
	{
		int chunkX = 0;// xCoord / Chunk::CHUNK_SIZE;
		int chunkY = 0;// yCoord / Chunk::CHUNK_SIZE;
		int dx = xCoord - chunkX * Chunk::CHUNK_SIZE;
		int dy = yCoord - chunkY * Chunk::CHUNK_SIZE;

		assert(mChunkMap[chunkX][chunkY]->mHeightMap[dx][dy] == mChunkMap[chunkX][chunkY]->mHeightMap[dx][dy]);

		return mChunkMap[chunkX][chunkY]->mHeightMap[dx][dy];
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};
};