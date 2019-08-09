#include "TerrainChunk.h"
#include "Terrain.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <algorithm>

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
			if (mpNChunk != nullptr)
			{
				mpNChunk->mpSChunk = nullptr;
			}
			if (mpWChunk != nullptr)
			{
				mpWChunk->mpEChunk = nullptr;
			}
			if (mpSChunk != nullptr)
			{
				mpSChunk->mpNChunk = nullptr;
			}
			if (mpEChunk != nullptr)
			{
				mpEChunk->mpWChunk = nullptr;
			}
		}

		void TerrainChunk::UpdateNormals()
		{
			// First, reset normals
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_SIZE; y++)
				{
					mNormalMap[x][y] = glm::vec3(0.f,0.f,0.f);
				}
			}

			// We will calulate the normal at each vertex by calculating the normal at all polygons and finding the average of the normals at neighboring polygons
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_SIZE; y++)
				{
					// Calculate positions of grid points defining this pair of triangles
					glm::vec3 topLeft = glm::vec3(mXCoord + x, mHeightMap[x][y], mYCoord + y);
					glm::vec3 bottomLeft = glm::vec3(mXCoord + x, mHeightMap[x][y + 1], mYCoord + y + 1);
					glm::vec3 bottomRight = glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][y + 1], mYCoord + y + 1);
					glm::vec3 topRight = glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][y], mYCoord + y);
					// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
					glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight)) / 6.f;
					glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

					// Add normals to average
					// Top Left
					mNormalMap[x][y] += normal1;
					// Top Right
					mNormalMap[x + 1][y] += normal1;
					mNormalMap[x + 1][y] += normal2;
					// Bottom Left
					mNormalMap[x][y + 1] += normal1;
					mNormalMap[x][y + 1] += normal2;
					// Top Left
					mNormalMap[x + 1][y + 1] += normal2;
				}
			}

			// Normalize normals
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_SIZE; y++)
				{
					mNormalMap[x][y] = glm::normalize(mNormalMap[x][y]);
				}
			}
		}

		void TerrainChunk::UpdateBorderVertices()
		{
			// EAST BORDER
			if (mpEChunk != nullptr)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE][i] = mpEChunk->mHeightMap[0][i];
					// Update border colors
					mColorMap[CHUNK_SIZE][i] = mpEChunk->mColorMap[0][i];
				}
			}
			// SOUTH BORDER
			if (mpSChunk != nullptr)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					// Update border heights
					mHeightMap[i][CHUNK_SIZE] = mpSChunk->mHeightMap[i][0];
					//mNormalMap[i][CHUNK_SIZE] = mpSChunk->mNormalMap[i][0];
					mColorMap[i][CHUNK_SIZE] = mpSChunk->mColorMap[i][0];
				}
			}
			// SOUTH-EASTERN CORNER
			if (mpSChunk != nullptr && mpSChunk->mpEChunk != nullptr)
			{
				// Update border heights
				mHeightMap[CHUNK_SIZE][CHUNK_SIZE] = mpSChunk->mpEChunk->mHeightMap[0][0];
				// Update border colors
				mColorMap[CHUNK_SIZE][CHUNK_SIZE] = mpSChunk->mpEChunk->mColorMap[0][0];
			}

			////We will have to update FOR ALL BORDERS too
			//// First, reset normals
			//for (int i = 0; i <= CHUNK_SIZE; i++)
			//{
			//	mNormalMap[0][i] = glm::vec3(0.f, 0.f, 0.f);
			//	mNormalMap[i][0] = glm::vec3(0.f, 0.f, 0.f);
			//	mNormalMap[CHUNK_SIZE][i] = glm::vec3(0.f, 0.f, 0.f);
			//	mNormalMap[i][CHUNK_SIZE] = glm::vec3(0.f, 0.f, 0.f);
			//}

			//// We will calulate the normal at each vertex by calculating the normal at all polygons and finding the average of the normals at neighboring polygons
			//// NORTH
			//for (int i = 0; i <= CHUNK_SIZE; i++)
			//{
			//	// Calculate positions of grid points defining this pair of triangles
			//	float y0 = mYCoord;
			//	float y1 = y0 + 1.f;
			//	glm::vec3 topLeft = glm::vec3(mXCoord + i, mHeightMap[i][CHUNK_SIZE - 1], y1);
			//	glm::vec3 bottomLeft = glm::vec3(mXCoord + i, mHeightMap[x][y + 1], y1);
			//	glm::vec3 bottomRight = glm::vec3(mXCoord + i + 1, mHeightMap[x + 1][y + 1], y1);
			//	glm::vec3 topRight = glm::vec3(mXCoord + i + 1, mHeightMap[x + 1][y], mYCoord + y);
			//	// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
			//	glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight)) / 6.f;
			//	glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

			//	// Add normals to average
			//	// Top Left
			//	mNormalMap[x][y] += normal1;
			//	// Top Right
			//	mNormalMap[x + 1][y] += normal1;
			//	mNormalMap[x + 1][y] += normal2;
			//	// Bottom Left
			//	mNormalMap[x][y + 1] += normal1;
			//	mNormalMap[x][y + 1] += normal2;
			//	// Top Left
			//	mNormalMap[x + 1][y + 1] += normal2;
			//}

			//// Normalize normals
			//for (int i = 0; i <= CHUNK_SIZE; i++)
			//{
			//	mNormalMap[0][i] = glm::normalize(mNormalMap[0][i]);
			//	mNormalMap[i][0] = glm::normalize(mNormalMap[i][0]);
			//	mNormalMap[CHUNK_SIZE][i] = glm::normalize(mNormalMap[CHUNK_SIZE][i]);
			//	mNormalMap[i][CHUNK_SIZE] = glm::normalize(mNormalMap[i][CHUNK_SIZE]);
			//}
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

					// Color will be average of vertex colors (one color per triangle)
					glm::vec3 color1 = glm::sqrt((mColorMap[x][y] * mColorMap[x][y] + mColorMap[x][y + stride] * mColorMap[x][y + stride] + mColorMap[x + stride][y] * mColorMap[x + stride][y]) / 3.f);
					glm::vec3 color2 = glm::sqrt((mColorMap[x + stride][y + stride] * mColorMap[x + stride][y + stride] + mColorMap[x][y + stride] * mColorMap[x][y + stride] + mColorMap[x + stride][y] * mColorMap[x + stride][y]) / 3.f);

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
			delete[] vertexBufer;

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

			//todo, make compatible with Aesthetic

			//Next, draw objects contained in the chunk
			for (auto it = objectsInChunk.begin(); it < objectsInChunk.end(); it++) 
			{
				(*it)->Draw();
			}

		}
		void TerrainChunk::AddObjectInstance(ChunkObjectInstance * i)
		{
			objectsInChunk.push_back(i);
		}
		void TerrainChunk::SortObjectInstances()
		{
			//The following lambda function sorts the object instance array, putting identical object types next to each other, minimizing the number of times we swap out the VBO and VAO.
			//todo test if this actually works properly
			std::sort(objectsInChunk.begin(), objectsInChunk.end(),
				[](const ChunkObjectInstance * a, const ChunkObjectInstance * b) -> bool
			{
				return a->GetModel() > b->GetModel();
			});
		}
	}
}