#pragma once

#include <cassert>
#include <glm/glm.hpp>

namespace pg
{
	namespace terrain
	{
		class Terrain;

		class TerrainChunk
		{
		public:
			// No default constructor because all chunks must belong to a Terrain object (I'm trying to emulate a Java nested class here)
			TerrainChunk() = delete;
			TerrainChunk(const Terrain&, const int xCoord, const int yCoord);
			TerrainChunk(const TerrainChunk& orig);

			~TerrainChunk();

			void UpdateNormals();
			// Generating the Vertex buffer for a Chunk will require that the chunk update it's south and eastern borders to match those of it's neighbors
			void UpdateBorderVertices();

			// Makes appropriate draw calls given current aesthetic (make sure that aesthetic is not changed between GenVertexBuffer() and this)
			void Draw();
			void SetLOD(const unsigned short LOD) 
			{ 
				assert(LOD < MIN_LOD); 
				if (mLOD != LOD)
				{
					mLOD = LOD;
					GenVertexBuffer();
				}
			}
			void SetVisibility(const bool isVisible) { mIsVisible = isVisible; }
			void SetNorthChunk(TerrainChunk* chunk) { SetNeighborChunk(0, chunk); }
			void SetWestChunk(TerrainChunk* chunk) { SetNeighborChunk(1, chunk); }
			void SetSouthChunk(TerrainChunk* chunk) { SetNeighborChunk(2, chunk); }
			void SetEastChunk(TerrainChunk* chunk) { SetNeighborChunk(3, chunk); }

			//Keep CHUNK_SIZE to a power of 2 (makes swapping LOD easier)
			const static int CHUNK_SIZE = 128;
			const static int MIN_LOD = 4;

			// We could make the code work without this rlationship, but it'll be more efficient for the Terrain and Terrain Generator classes if they can directly access the chunk data
			friend class Terrain;
			friend class TerrainGenerator;

		private:
			// Chunks will always share a set of vertices with neighboring chunks (this is inevitable)
			// We can't let both chunks have authority over these vertices (to do so would require more checks for updating those vertices)
			// Instead, each chunk will have authority over it's northern and western border
			// Generates appropriate mesh given current aesthetic (make sure that terrain aesthtic is set before calling this)
			void GenVertexBuffer();
			//Generates VAO for low-poly aesthetic
			void GenVertexBufferLowPoly();
			//Generates VAO for smooth aesthetic
			void GenVertexBufferSmooth();
			void BindAttributes();

			// Set neighbor identified by index to chunk
			void SetNeighborChunk(const unsigned short index, TerrainChunk* chunk);

			// Terrain that chunk belongs to
			const Terrain& mTerrain;
			// NEIGHBOR DATA (probably faster then searching map)
			// Also used to generate vertices for border
			// North is -z direction
			// East is +x direction
			TerrainChunk* mpNChunk = nullptr;
			TerrainChunk* mpWChunk = nullptr;
			TerrainChunk* mpSChunk = nullptr;
			TerrainChunk* mpEChunk = nullptr;

			// RENDER DATA
			unsigned int mVAO = 0;
			unsigned int mVBO = 0;
			// Initial value for LOD is invalid. Mesh will be drawn once LOD is set to a valid value.
			unsigned short mLOD = MIN_LOD;
			bool mIsVisible = false;

			// MESH DATA
			const int mXCoord;
			const int mYCoord;
			float mHeightMap[CHUNK_SIZE + 1][CHUNK_SIZE + 1];
			glm::vec3 mNormalMap[CHUNK_SIZE + 1][CHUNK_SIZE + 1];
			glm::vec3 mColorMap[CHUNK_SIZE + 1][CHUNK_SIZE + 1];

			struct Vertex
			{
				glm::vec3 mPosition;
				glm::vec3 mNormal;
				glm::vec3 mColor;
			};
		};
	}
}