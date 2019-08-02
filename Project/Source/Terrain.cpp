#include "Terrain.h"
#include "Renderer.h"
#include "PerlinNoise.h"
#include <iostream>

PerlinNoise Terrain::Chunk::noiseGenerator = PerlinNoise();

Terrain::Chunk::Chunk()
{
}

float Ease(float t)
{
	const float exp = 5.f;
	return (t < 0.5f) ? 0.5f * glm::pow(2.f * t, exp) : 0.5f * glm::pow(2.f * t - 2.f, exp) + 1.f;
}

Terrain::Chunk::Chunk(int xCoord, int yCoord)
{
	float amplitude = 20.f;
	float frequency = 1.f/128.f;

	unsigned int numOctaves = 5;
	float lacunarity = 3.f;
	float persistence = 0.4f;

	mXCoord = xCoord;
	mYCoord = yCoord;

	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			mHeightMap[i][j] = amplitude * Ease((Chunk::noiseGenerator.Perlin(frequency * static_cast <float> (mXCoord + i), frequency *  static_cast <float> (mYCoord + j)))/2.f + 0.5f);
		}
	}
	amplitude *= persistence;
	frequency *= lacunarity;

	for (int octave = 0; octave < numOctaves; octave++)
	{
		for (int i = 0; i < CHUNK_SIZE; i++)
		{
			for (int j = 0; j < CHUNK_SIZE; j++)
			{
				mHeightMap[i][j] += amplitude * Chunk::noiseGenerator.Perlin(frequency * static_cast <float> (mXCoord + i), frequency *  static_cast <float> (mYCoord + j));
			}
		}
		amplitude *= persistence;
		frequency *= lacunarity;
	}

	GenVertexBuffer();
}

Terrain::Chunk::~Chunk()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void Terrain::Chunk::GenVertexBuffer()
{
	float scale = 0.15f;

	Vertex* vertexBufer = new Vertex[2 * (CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * 3];
	int crrtVertex = 0;

	for (int x = 0; x < CHUNK_SIZE - 1; x++)
	{
		for (int y = 0; y < CHUNK_SIZE - 1; y++)
		{
			glm::vec3 topLeft = scale * glm::vec3(mXCoord + x, mHeightMap[x][y], mYCoord + y);
			glm::vec3 bottomLeft = scale * glm::vec3(mXCoord + x, mHeightMap[x][y + 1], mYCoord + y + 1);
			glm::vec3 bottomRight = scale * glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][y + 1], mYCoord + y + 1);
			glm::vec3 topRight = scale * glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][y], mYCoord + y);

			glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight));
			glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight));

			float verticality = glm::dot(normal1, glm::vec3(0.f, 1.f, 0.f));
			glm::vec3 color1 = glm::mix(glm::vec3(0.50f, 0.31f, 0.24f), glm::vec3(0.55f, 0.62f, 0.25f), glm::pow(verticality, 5.f));
			verticality = glm::dot(normal2, glm::vec3(0.f, 1.f, 0.f));
			glm::vec3 color2 = glm::mix(glm::vec3(0.50f, 0.31f, 0.24f), glm::vec3(0.55f, 0.62f, 0.25f), glm::pow(verticality, 5.f));

			vertexBufer[crrtVertex++] = Vertex{ topRight, normal1, color1 };
			vertexBufer[crrtVertex++] = Vertex{ topLeft, normal1, color1 };
			vertexBufer[crrtVertex++] = Vertex{ bottomLeft, normal1, color1 };

			vertexBufer[crrtVertex++] = Vertex{ topRight, normal2, color2 };
			vertexBufer[crrtVertex++] = Vertex{ bottomLeft, normal2, color2 };
			vertexBufer[crrtVertex++] = Vertex{ bottomRight, normal2, color2 };
		}
	}

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, crrtVertex * sizeof(Vertex), vertexBufer, GL_STATIC_DRAW);

	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                // size
		GL_FLOAT,        // type
		GL_FALSE,        // normalized?
		sizeof(Vertex), // stride
		(void*)0        // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normal
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)sizeof(glm::vec3)    // Normal is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);

	// 3rd attribute buffer : vertex color
	glVertexAttribPointer(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(2);
}

void Terrain::Chunk::Draw()
{
	// Draw the Vertex Buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 worldMatrix(1.0f);
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// Draw the terrain
	glDrawArrays(GL_TRIANGLES, 0, 2 * (CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * 3);
}

Terrain::Terrain() : Terrain(1,1)
{
}

Terrain::Terrain(unsigned int width, unsigned int height) : mWidth(width), mHeight(height)
{
	mChunkMap = new Chunk**[mWidth];
	for (int i = 0; i < mWidth; i++)
	{
		mChunkMap[i] = new Chunk*[mHeight];
		for (int j = 0; j < mHeight; j++)
		{
			mChunkMap[i][j] = new Chunk{ i*(Chunk::CHUNK_SIZE-1), j*(Chunk::CHUNK_SIZE-1) };
		}
	}
}

Terrain::~Terrain()
{
	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			delete mChunkMap[i][j];
		}
		delete [] mChunkMap[i];
	}
	delete [] mChunkMap;
}

void Terrain::Draw()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			mChunkMap[i][j]->Draw();
		}
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}