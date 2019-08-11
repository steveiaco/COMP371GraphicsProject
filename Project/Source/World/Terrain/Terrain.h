#pragma once

#include "TerrainChunk.h"
#include "../../ChunkPopulator.h"
#include "../Water/WaterRenderer.h"


#include <map>

namespace pg
{
	namespace terrain
	{
		class TerrainGenerator;

		enum TerrainAesthetic
		{
			LOW_POLY,
			SMOOTH,
			WIREFRAME
		};

		class Terrain
		{
		public:
			Terrain() = delete;
			Terrain(const TerrainGenerator& terrainGenerator);
			Terrain(const Terrain& orig);

			~Terrain();

			// Get interpolated height
			float GetHeightAt(const float xCoord, const float yCoord) const;
			// Get height at vertex
			float GetHeightAt(const int xCoord, const int yCoord) const;
			// Get height at vertex
			float* GetHeightRefAt(const int xCoord, const int yCoord);
			// Set height at vertex
			void SetHeightAt(const int xCoord, const int yCoord, const float value);
			// Get interpolated normal
			glm::vec3 GetNormalAt(const float xCoord, const float yCoord) const;
			// Get humidity at coordinates (used to determine biome)
			float GetHumidityAt(const int mXCoord, const int mYCoord) const;
			// Get temperature at coordinates (used to determine biome)
			float GetTemperatureAt(const int mXCoord, const int mYCoord) const;

			// Get rendering aesthetic to use
			TerrainAesthetic GetAesthetic() const { return mAesthetic; }
			const TerrainGenerator& GetTerrainGenerator() const { return mTerrainGenerator; }
			void SetAesthetic(const TerrainAesthetic aesthetic) { mAesthetic = aesthetic; }

			// Attach ChunkPopulator (MUST BE RUN BEFORE USING THIS CLASS)
			void AttachChunkPopulator(ChunkPopulator* c);
			
			//Add chunk object to chunk populator
			void AddChunkObject(ChunkObject * o);

			//Start terrain generation, used to synchronize object loading (so that chunks do not start getting generated before all other setup is complete)
			void Start();

			//Draw visible chunks. Create chunks and update their LOD as needed. We do updates within draw to avoid having to look-up nearby chunks more than once.
			void Draw();
			void DrawWater(water::WaterRenderer& waterRenderer);

		private:

			const static bool GenerateInfiniteTerrain = false;

			// Get normal at vertex
			glm::vec3 GetNormalAt(const int xCoord, const int yCoord) const;

			// Will get chunk at coordinates if it exists, will create it otherwise
			TerrainChunk& GetChunkAt(const int xCoord, const int yCoord);

			//Used to populate new chunks with objects
			ChunkPopulator* chunkPopulator;
			// Generator used to generate new chunks
			const TerrainGenerator& mTerrainGenerator;

			//Terrain aesthetic used for rendering
			TerrainAesthetic mAesthetic = LOW_POLY;
			// Map of previously generated chunks
			std::map<std::pair<int, int>, TerrainChunk*> mChunkMap = std::map<std::pair<int, int>, TerrainChunk*>();

			bool initialGenDone;
		};
	}
}
