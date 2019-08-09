#pragma once
#include <vector>
#include "ChunkObject.h"
#include "PerlinNoise.h"
#include "World/Terrain/TerrainChunk.h"
#include "World/Terrain/Terrain.h"

namespace pg 
{
	namespace terrain 
	{
		class ChunkPopulator 
		{
		public:
			ChunkPopulator(const Terrain & t, const PerlinNoise & p);
		
			void AddObject(ChunkObject* c);
			void PopulateChunk(TerrainChunk* t);
		
		private:

			const static int STRIDE_X = 1;
			const static int STRIDE_Y = 1;

			const PerlinNoise& mNoise;
			const Terrain& mTerrain;
			std::vector<ChunkObject*> chunkObjects;

			bool VerifyObjectPlacement(ChunkObject* o, const float x, const float y);
		};
	}
}
