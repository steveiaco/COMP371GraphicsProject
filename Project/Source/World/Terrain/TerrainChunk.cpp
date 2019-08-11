#include "TerrainChunk.h"
#include "TerrainGenerator.h"
#include "Terrain.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace pg
{
	namespace terrain
	{
		TerrainChunk::TerrainChunk(const Terrain& terrain, const int xCoord, const int yCoord)
			: mXCoord(xCoord)
			, mYCoord(yCoord)
			, mTerrain(terrain)
			, mWater(xCoord, 0.f, yCoord, CHUNK_SIZE, CHUNK_SIZE)
		{
		}

		TerrainChunk::TerrainChunk(const TerrainChunk& orig)
			: mXCoord(orig.mXCoord)
			, mYCoord(orig.mYCoord)
			, mTerrain(orig.mTerrain)
			, mWater(orig.mWater)
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
			glDeleteBuffers(MIN_LOD, mVBOV);
			glDeleteBuffers(MIN_LOD, mVBOI);
			glDeleteVertexArrays(MIN_LOD, mVAO);

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
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				for (int y = 0; y < CHUNK_SIZE + 1; y++)
				{
					mNormalMap[x][y] = glm::vec3(0.f,0.f,0.f);
				}
			}

			// We will calulate the normal at each vertex by calculating the normal at all polygons and finding the average of the normals at neighboring polygons
			for (int x = 0; x < CHUNK_SIZE + 2; x++)
			{
				for (int y = 0; y < CHUNK_SIZE + 2; y++)
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
					// Bottom Right
					mNormalMap[x + 1][y + 1] += normal2;
				}
			}

			// We still need to find the normals of the polygons bordering the nortern and western vertices...
			float y0 = mYCoord - 1.f;
			float y1 = mYCoord;
			float x0 = mXCoord - 1.f;
			float x1 = mXCoord;

			// NORTH-EAST
			// Calculate positions of grid points defining this pair of triangles
			glm::vec3 topLeft = glm::vec3(mXCoord + CHUNK_SIZE, mHeightMap[CHUNK_SIZE][CHUNK_SIZE + 2], y0);
			glm::vec3 bottomLeft = glm::vec3(mXCoord + CHUNK_SIZE, mHeightMap[CHUNK_SIZE][0], y1);
			glm::vec3 bottomRight = glm::vec3(mXCoord + CHUNK_SIZE + 1, mHeightMap[CHUNK_SIZE + 1][0], y1);
			glm::vec3 topRight = glm::vec3(mXCoord + CHUNK_SIZE + 1, mHeightMap[CHUNK_SIZE + 1][CHUNK_SIZE + 2], y0);
			//// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
			glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight)) / 6.f;
			glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

			// Add normals to average
			// Bottom Left
			mNormalMap[CHUNK_SIZE][0] += normal1;
			mNormalMap[CHUNK_SIZE][0] += normal2;

			// NORTH
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				// Calculate positions of grid points defining this pair of triangles
				glm::vec3 topLeft = glm::vec3(mXCoord + x, mHeightMap[x][CHUNK_SIZE + 2], y0);
				glm::vec3 bottomLeft = glm::vec3(mXCoord + x, mHeightMap[x][0], y1);
				glm::vec3 bottomRight = glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][0], y1);
				glm::vec3 topRight = glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][CHUNK_SIZE + 2], y0);
				// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
				glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight)) / 6.f;
				glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

				// Add normals to average
				// Bottom Left
				mNormalMap[x][0] += normal1;
				mNormalMap[x][0] += normal2;
				// Top Left
				mNormalMap[x + 1][0] += normal2;
			}

			// NORTH-WEST
			// Calculate positions of grid points defining this pair of triangles
			bottomLeft = glm::vec3(x0, mHeightMap[CHUNK_SIZE + 2][0], y1);
			bottomRight = glm::vec3(x1, mHeightMap[0][0], y1);
			topRight = glm::vec3(x1, mHeightMap[0][CHUNK_SIZE + 2], y0);
			// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
			normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

			// Add normals to average
			// Bottom Left
			mNormalMap[0][0] += normal2;

			// WEST
			for (int y = 0; y < CHUNK_SIZE + 1; y++)
			{
				// Calculate positions of grid points defining this pair of triangles
				glm::vec3 topLeft = glm::vec3(x0, mHeightMap[CHUNK_SIZE + 2][y], mYCoord + y);
				glm::vec3 bottomLeft = glm::vec3(x0, mHeightMap[CHUNK_SIZE + 2][y + 1], mYCoord + y + 1);
				glm::vec3 bottomRight = glm::vec3(x1, mHeightMap[0][y + 1], mYCoord + y + 1);
				glm::vec3 topRight = glm::vec3(x1, mHeightMap[0][y], mYCoord + y);
				// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
				glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight)) / 6.f;
				glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

				// Add normals to average
				// Top Right
				mNormalMap[0][y] += normal1;
				mNormalMap[0][y] += normal2;
				// Bottom Right
				mNormalMap[0][y + 1] += normal2;
			}

			// SOUTH-WEST
			// Calculate positions of grid points defining this pair of triangles
			topLeft = glm::vec3(x0, mHeightMap[CHUNK_SIZE + 2][CHUNK_SIZE], mXCoord + CHUNK_SIZE);
			bottomLeft = glm::vec3(x0, mHeightMap[CHUNK_SIZE + 2][CHUNK_SIZE + 1], mXCoord + CHUNK_SIZE + 1);
			bottomRight = glm::vec3(x1, mHeightMap[0][CHUNK_SIZE + 1], mXCoord + CHUNK_SIZE + 1);
			topRight = glm::vec3(x1, mHeightMap[0][CHUNK_SIZE], mXCoord + CHUNK_SIZE);
			// Caclulate contribution to average normal (we divide by 6 because each vertex has 6 neighboring polygons)
			normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight)) / 6.f;
			normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight)) / 6.f;

			// Add normals to average
			// Top Right
			mNormalMap[0][CHUNK_SIZE] += normal1;
			mNormalMap[0][CHUNK_SIZE] += normal2;

			// Normalize normals
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				for (int y = 0; y < CHUNK_SIZE + 1; y++)
				{
					mNormalMap[x][y] = glm::normalize(mNormalMap[x][y]);
				}
			}
		}

		void TerrainChunk::UpdateBorderVertices()
		{
			// NORTH BORDER
			if (mpNChunk != nullptr)
			{
				// NORTH-EASTERN CORNER
				if (mpNChunk->mpEChunk != nullptr)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE + 1][CHUNK_SIZE + 2] = mpNChunk->mpEChunk->mHeightMap[1][CHUNK_SIZE - 1];
					mHeightMap[CHUNK_SIZE][CHUNK_SIZE + 2] = mpNChunk->mpEChunk->mHeightMap[0][CHUNK_SIZE - 1];
				}
				// NORTH
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					// Update border heights
					mHeightMap[i][CHUNK_SIZE + 1] = mpNChunk->mHeightMap[i][CHUNK_SIZE - 1];
				}
				// NORTH-WESTERN CORNER
				if (mpNChunk->mpWChunk != nullptr)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE + 2][CHUNK_SIZE + 2] = mpNChunk->mpWChunk->mHeightMap[CHUNK_SIZE - 1][CHUNK_SIZE - 1];
				}
			}
			// WEST BORDER
			if (mpWChunk != nullptr)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE + 2][i] = mpWChunk->mHeightMap[CHUNK_SIZE - 1][i];
				}
			}
			// SOUTH BORDER
			if (mpSChunk != nullptr)
			{
				// SOUTH_WESTERN BORDER
				if (mpSChunk->mpWChunk != nullptr)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE + 2][CHUNK_SIZE] = mpSChunk->mpWChunk->mHeightMap[CHUNK_SIZE - 1][0];
					mHeightMap[CHUNK_SIZE + 2][CHUNK_SIZE + 1] = mpSChunk->mpWChunk->mHeightMap[CHUNK_SIZE - 1][1];
				}
				// SOUTH
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					// Update border heights
					mHeightMap[i][CHUNK_SIZE] = mpSChunk->mHeightMap[i][0];
					mHeightMap[i][CHUNK_SIZE + 1] = mpSChunk->mHeightMap[i][1];
					// Update border colors
					mColorMap[i][CHUNK_SIZE] = mpSChunk->mColorMap[i][0];
				}
				// SOUTH-EASTERN CORNER
				if (mpSChunk->mpEChunk != nullptr)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE][CHUNK_SIZE] = mpSChunk->mpEChunk->mHeightMap[0][0];
					mHeightMap[CHUNK_SIZE + 1][CHUNK_SIZE] = mpSChunk->mpEChunk->mHeightMap[1][0];
					mHeightMap[CHUNK_SIZE + 1][CHUNK_SIZE + 1] = mpSChunk->mpEChunk->mHeightMap[1][1];
					mHeightMap[CHUNK_SIZE][CHUNK_SIZE + 1] = mpSChunk->mpEChunk->mHeightMap[0][1];
					// Update border colors
					mColorMap[CHUNK_SIZE][CHUNK_SIZE] = mpSChunk->mpEChunk->mColorMap[0][0];
				}
			}
			// EAST BORDER
			if (mpEChunk != nullptr)
			{
				for (int i = 0; i < CHUNK_SIZE; i++)
				{
					// Update border heights
					mHeightMap[CHUNK_SIZE][i] = mpEChunk->mHeightMap[0][i];
					mHeightMap[CHUNK_SIZE + 1][i] = mpEChunk->mHeightMap[1][i];
					// Update border colors
					mColorMap[CHUNK_SIZE][i] = mpEChunk->mColorMap[0][i];
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

		void TerrainChunk::GenVertexBuffers()
		{
			// Free the GPU from the Vertex Buffer
			glDeleteBuffers(MIN_LOD, mVBOV);
			glDeleteBuffers(MIN_LOD, mVBOI);
			glDeleteVertexArrays(MIN_LOD, mVAO);

			UpdateBorderVertices();
			UpdateNormals();
			mTerrain.GetTerrainGenerator().ColorChunk(*this);

			glGenVertexArrays(MIN_LOD, mVAO);
			glGenBuffers(MIN_LOD, mVBOI);
			glGenBuffers(MIN_LOD, mVBOV);

			//Different aesthetics might require different vertex buffer objects
			switch (mTerrain.GetAesthetic())
			{
			case LOW_POLY:
				for (unsigned short lod = 0; lod < MIN_LOD; lod++)
				GenVertexBufferLowPoly(lod);
				break;
			case SMOOTH:
				for (unsigned short lod = 0; lod < MIN_LOD; lod++)
				GenVertexBufferSmooth(lod);
				break;
			case WIREFRAME:
				for (unsigned short lod = 0; lod < MIN_LOD; lod++)
				GenVertexBufferLowPoly(lod);
				break;
			}
		}

		void TerrainChunk::GenVertexBufferLowPoly(unsigned short lod)
		{
			// Calculations are simplest if stride is a power of 2 (assumign that CHUNK_SIZE is also a power of 2)
			int stride = glm::pow(2, lod);
			// We will generate three vertices for every triangle (so that the color and normal of every triangle is flat)
			// This will give us a low-poly aesthetic
			// We will build the VAO within this array, two triangles at a time
			Vertex* vertexBufer = new Vertex[ 2 * (CHUNK_SIZE / stride) * (CHUNK_SIZE / stride) * 3 ]; //Has to be heap-allocated, was getting stack overflows otherwise
			int crrtVertex = 0;

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
			glBindVertexArray(mVAO[lod]);

			// Upload Vertex Buffer to the GPU, keep a reference to it
			glBindBuffer(GL_ARRAY_BUFFER, mVBOV[lod]);
			glBufferData(GL_ARRAY_BUFFER, crrtVertex * sizeof(Vertex), vertexBufer, GL_STATIC_DRAW);
			delete[] vertexBufer;

			//Bind layout attributes
			BindAttributes();
		}

		void TerrainChunk::GenVertexBufferSmooth(unsigned short lod)
		{
			// Similar procedure to low-poly
			int stride = glm::pow(2, lod);
			Vertex* vertexBufer = new Vertex[(CHUNK_SIZE / stride + 1) * (CHUNK_SIZE / stride + 1)];
			unsigned int* indexBuffer = new unsigned int[2 * (CHUNK_SIZE / stride) * (CHUNK_SIZE / stride) * 3];
			int crrtVertex = 0;

			// Fill vertex array
			for (int x = 0; x < CHUNK_SIZE + 1; x += stride)
			{
				for (int y = 0; y < CHUNK_SIZE + 1; y += stride)
				{
					// Calculate positions of grid points defining this pair of triangles
					glm::vec3 pos = glm::vec3(mXCoord + x, mHeightMap[x][y], mYCoord + y);
					//Add vertex
					vertexBufer[crrtVertex++] = Vertex{ pos, mNormalMap[x][y], mColorMap[x][y] };
				}
			}

			// Fill index array
			crrtVertex = 0;
			for (unsigned int x = 0; x < CHUNK_SIZE/stride; x++)
			{
				for (unsigned int y = 0; y < CHUNK_SIZE / stride; y++)
				{
					//NW TRIANGLE
					indexBuffer[crrtVertex++] = (x + 1) * (CHUNK_SIZE / stride + 1) + y;
					indexBuffer[crrtVertex++] = x * (CHUNK_SIZE / stride + 1) + y;
					indexBuffer[crrtVertex++] = x * (CHUNK_SIZE / stride + 1) + y + 1;
					//SE TRIANGLE
					indexBuffer[crrtVertex++] = x * (CHUNK_SIZE / stride + 1) + y + 1;
					indexBuffer[crrtVertex++] = (x + 1) * (CHUNK_SIZE / stride + 1) + y + 1;
					indexBuffer[crrtVertex++] = (x + 1) * (CHUNK_SIZE / stride + 1) + y;
				}
			}

			// Create a vertex array
			glBindVertexArray(mVAO[lod]);

			// Upload Vertex Buffers to the GPU, keep references to them
			glBindBuffer(GL_ARRAY_BUFFER, mVBOV[lod]);
			glBufferData(GL_ARRAY_BUFFER, (CHUNK_SIZE / stride + 1) * (CHUNK_SIZE / stride + 1) * sizeof(Vertex), vertexBufer, GL_STATIC_DRAW);
			delete[] vertexBufer;

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBOI[lod]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, crrtVertex * sizeof(unsigned int), indexBuffer, GL_STATIC_DRAW);
			delete[] indexBuffer;

			//Bind layout attributes
			BindAttributes();
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

		void TerrainChunk::Draw(unsigned short lod)
		{
			assert(lod < MIN_LOD);
			if (mVAO[lod] == 0)
			{
				GenVertexBuffers();
			}

			// Draw the Vertex Buffer
			int stride = glm::pow(2, lod);
			glBindVertexArray(mVAO[lod]);
			glBindBuffer(GL_ARRAY_BUFFER, mVBOV[lod]);

			// Draw the terrain
			switch (mTerrain.GetAesthetic())
			{
			case SMOOTH:
				glBindBuffer(GL_ARRAY_BUFFER, mVBOI[lod]);
				glDrawElements(GL_TRIANGLES, 2 * (CHUNK_SIZE / stride) * (CHUNK_SIZE / stride) * 3, GL_UNSIGNED_INT, (void*) 0);
				break;
			case LOW_POLY:
				glDrawArrays(GL_TRIANGLES, 0, 2 * (CHUNK_SIZE / stride) * (CHUNK_SIZE / stride) * 3);
				break;
			case WIREFRAME:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glBindBuffer(GL_ARRAY_BUFFER, mVBOI[lod]);
				glDrawElements(GL_TRIANGLES, 2 * (CHUNK_SIZE / stride) * (CHUNK_SIZE / stride) * 3, GL_UNSIGNED_INT, (void*)0);
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