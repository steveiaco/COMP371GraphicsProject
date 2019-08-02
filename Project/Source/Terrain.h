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