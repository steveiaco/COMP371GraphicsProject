#include "TerrainChunk.h"
#include "Terrain.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace pg
{
	namespace terrain
	{
		TerrainChunk::TerrainChunk(const Terrain& terrain, const int mXCoord, const int mYCoord)
			: mXCoord(mXCoord)
			, mYCoord(mYCoord)
			, mTerrain(terrain)
		{
			// Uncomment for testing flat meshes
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				for (int y = 0; y < CHUNK_SIZE + 1; y++)
				{
					mHeightMap[x][y] = 0.f;
					mNormalMap[x][y] = {0.f, 1.f, 0.f};
					mColorMap[x][y] = {0.5f, 0.5f, 0.5f};
				}
			}
		}

		TerrainChunk::TerrainChunk(const TerrainChunk& orig)
			: mXCoord(orig.mXCoord)
			, mYCoord(orig.mYCoord)
			, mTerrain(orig.mTerrain)
		{
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				for (int y = 0; y < CHUNK_SIZE + 1; y++)
				{
					mHeightMap[x][y] = orig.mHeightMap[x][y];
					mNormalMap[x][y] = orig.mNormalMap[x][y];
					mColorMap[x][y] = orig.mColorMap[x][y];
				}
			}
		}

		TerrainChunk::~TerrainChunk()
		{
			// Free the GPU from the Vertex Buffer
			glDeleteBuffers(1, &mVBO);
			glDeleteVertexArrays(1, &mVAO);

			//Remove reference to self from neighboring chunks
			mpNChunk->mpSChunk = nullptr;
			mpWChunk->mpEChunk = nullptr;
			mpSChunk->mpNChunk = nullptr;
			mpEChunk->mpWChunk = nullptr;
		}

		void TerrainChunk::UpdateBorderVertices()
		{
			//EAST BORDER
			if (mpEChunk != nullptr)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					mHeightMap[CHUNK_SIZE][i] = mpEChunk->mHeightMap[0][i];
					mNormalMap[CHUNK_SIZE][i] = mpEChunk->mNormalMap[0][i];
					mColorMap[CHUNK_SIZE][i] = mpEChunk->mColorMap[0][i];
				}
			}
			//SOUTH BORDER
			if (mpSChunk != nullptr)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					mHeightMap[i][CHUNK_SIZE] = mpSChunk->mHeightMap[i][0];
					mNormalMap[i][CHUNK_SIZE] = mpSChunk->mNormalMap[i][0];
					mColorMap[i][CHUNK_SIZE] = mpSChunk->mColorMap[i][0];
				}
			}
		}

		void TerrainChunk::SetNeighborChunk(const unsigned short index, TerrainChunk* chunk)
		{
			//Different aesthetics might require different vertex buffer objects
			switch (index)
			{
			case 0:
				mpNChunk = chunk;
				assert(mpNChunk->mpSChunk == nullptr);
				mpNChunk->mpSChunk = this;
				break;
			case 1:
				mpWChunk = chunk;
				assert(mpWChunk->mpEChunk == nullptr);
				mpWChunk->mpEChunk = this;
				break;
			case 2:
				mpSChunk = chunk;
				assert(mpSChunk->mpNChunk == nullptr);
				mpSChunk->mpNChunk = this;
				break;
			case 3:
				mpEChunk = chunk;
				assert(mpEChunk->mpWChunk == nullptr);
				mpEChunk->mpWChunk = this;
				break;
			}
		}

		void TerrainChunk::GenVertexBuffer()
		{
			// Free the GPU from the Vertex Buffer
			glDeleteBuffers(1, &mVBO);
			glDeleteVertexArrays(1, &mVAO);

			UpdateBorderVertices();

			//Different aesthetics might require different vertex buffer objects
			switch (mTerrain.GetAesthetic())
			{
			case LOW_POLY:
				GenVertexBufferLowPoly();
				break;
			case SMOOTH:
				GenVertexBufferSmooth();
				break;
			case WIREFRAME:
				GenVertexBufferLowPoly();
				break;
			}
		}

		void TerrainChunk::GenVertexBufferLowPoly()
		{
			// We will generate three vertices for every triangle (so that the color and normal of every triangle is flat)
			// This will give us a low-poly aesthetic
			// We will build the VAO within this array, two triangles at a time
			Vertex* vertexBufer = new Vertex[ 2 * CHUNK_SIZE * CHUNK_SIZE * 3 ]; //Has to be heap-allocated, was getting stack overflows otherwise
			int crrtVertex = 0;

			// Calculations are simplest if stride is a power of 2 (assumign that CHUNK_SIZE is also a power of 2)
			int stride = glm::pow(2, mLOD);
			for (int x = 0; x < CHUNK_SIZE; x += stride)
			{
				for (int y = 0; y < CHUNK_SIZE; y += stride)
				{
					// Calculate positions of grid points defining this pair of triangles
					glm::vec3 topLeft = glm::vec3(mXCoord + x, mHeightMap[x][y], mYCoord + y);
					glm::vec3 bottomLeft = glm::vec3(mXCoord + x, mHeightMap[x][y + stride], mYCoord + y + stride);
					glm::vec3 bottomRight = glm::vec3(mXCoord + x + stride, mHeightMap[x + stride][y + stride], mYCoord + y + stride);
					glm::vec3 topRight = glm::vec3(mXCoord + x + stride, mHeightMap[x + stride][y], mYCoord + y);

					// Calculate normals for the triangles (one normal per triangle)
					glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight));
					glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight));

					// Color will be average of vertex colors (one color per triangle)float verticality = glm::dot(normal1, glm::vec3(0.f, 1.f, 0.f));
					float verticality = glm::dot(normal1, glm::vec3(0.f, 1.f, 0.f));
					glm::vec3 color1 = glm::mix(glm::vec3(0.85f, 0.56f, 0.46f), glm::vec3(0.92f, 1.f, 0.48), glm::pow(verticality, 5.f));
					verticality = glm::dot(normal2, glm::vec3(0.f, 1.f, 0.f));
					glm::vec3 color2 = glm::mix(glm::vec3(0.85f, 0.56f, 0.46f), glm::vec3(0.92f, 1.f, 0.48f), glm::pow(verticality, 5.f));
					//glm::vec3 color1 = glm::sqrt((mColorMap[x][y] * mColorMap[x][y] + mHeightMap[x][y + stride] * mHeightMap[x][y + stride] + mColorMap[x + stride][y] * mColorMap[x + stride][y]) / 3.f);
					//glm::vec3 color2 = glm::sqrt((mColorMap[x + stride][y + stride] * mColorMap[x + stride][y + stride] + mHeightMap[x][y + stride] * mHeightMap[x][y + stride] + mColorMap[x + stride][y] * mColorMap[x + stride][y]) / 3.f);

					//Add vertices for triangle 1
					vertexBufer[crrtVertex++] = Vertex{ topRight, normal1, color1 };
					vertexBufer[crrtVertex++] = Vertex{ topLeft, normal1, color1 };
					vertexBufer[crrtVertex++] = Vertex{ bottomLeft, normal1, color1 };

					//Add vertices for triangle 2
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
			delete vertexBufer;

			//Bind layout attributes
			BindAttributes();
		}

		void TerrainChunk::GenVertexBufferSmooth()
		{
			//TODO
		}

		void TerrainChunk::BindAttributes()
		{
			// Vertex Position
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(Vertex),
				(void*)0
			);
			glEnableVertexAttribArray(0);

			// Vertex Normal
			glVertexAttribPointer(
				1,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(Vertex),
				(void*)sizeof(glm::vec3)
			);
			glEnableVertexAttribArray(1);

			// Vertex Color
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				sizeof(Vertex),
				(void*)(2 * sizeof(glm::vec3))
			);
			glEnableVertexAttribArray(2);
		}

		void TerrainChunk::Draw()
		{
			// Draw the Vertex Buffer
			glBindVertexArray(mVAO);
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);

			// Draw the terrain
			switch (mTerrain.GetAesthetic())
			{
			case SMOOTH:
				//TODO
				exit(1);
			case LOW_POLY:
				glDrawArrays(GL_TRIANGLES, 0, 2 * CHUNK_SIZE * CHUNK_SIZE * 3);
				break;
			case WIREFRAME:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawArrays(GL_TRIANGLES, 0, 2 * CHUNK_SIZE * CHUNK_SIZE * 3);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}
		}
	}
}