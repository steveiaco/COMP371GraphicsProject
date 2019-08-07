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
		const static int CHUNK_SIZE = 257;

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

	float GetHeight(float xCoord, float yCoord) const;
	//To get the height at a node, we will have to find the chunk that said node belongs to and the position of the node within that chunk
	float GetVertexHeight(int xCoord, int yCoord) const
	{
		int chunkX = xCoord / Chunk::CHUNK_SIZE;
		int chunkY = yCoord / Chunk::CHUNK_SIZE;
		int dx = xCoord - chunkX * (Chunk::CHUNK_SIZE - 1);
		int dy = yCoord - chunkY * (Chunk::CHUNK_SIZE - 1);

		return mChunkMap[chunkX][chunkY]->mHeightMap[dx][dy];
	};

private:
	const unsigned int mWidth;
	const unsigned int mHeight;
	Chunk*** mChunkMap;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};
};