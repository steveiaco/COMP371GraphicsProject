#pragma once

#include <glm/glm.hpp>

class Terrain {

private:
	class Chunk {
	public:
		Chunk();
		Chunk(int xCoord, int yCoord);
		~Chunk();

		void GenVertexBuffer();
		void Draw();

		unsigned int mVAO = 55;
		unsigned int mVBO = 55;

		int mXCoord;
		int mYCoord;
		float** mHeightMap;
	};

public:
	const static int CHUNK_SIZE = 16;

	Terrain();
	Terrain(unsigned int width, unsigned int height);
	~Terrain();

	void Draw();

private:
	const unsigned int mWidth;
	const unsigned int mHeight;
	Chunk** mChunkMap;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};
};