#pragma once

#include "TerrainChunk.h"

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
			// Get interpolated normal
			glm::vec3 GetNormalAt(const float xCoord, const float yCoord) const;
			// Get humidity at coordinates (used to determine biome)
			float GetHumidityAt(const int mXCoord, const int mYCoord) const;
			// Get temperature at coordinates (used to determine biome)
			float GetTemperatureAt(const int mXCoord, const int mYCoord) const;

			// Get rendering aesthetic to use
			TerrainAesthetic GetAesthetic() const { return mAesthetic; }
			void SetAesthetic(const TerrainAesthetic aesthetic) { mAesthetic = aesthetic; }

			//Draw visible chunks. Create chunks and update their LOD as needed. We do updates within draw to avoid having to look-up nearby chunks more than once.
			void Draw();

		private:
			// Get height at vertex
			float GetHeightAt(const int xCoord, const int yCoord) const;
			// Get normal at vertex
			glm::vec3 GetNormalAt(const int xCoord, const int yCoord) const;

			// Will get chunk at coordinates if it exists, will create it otherwise
			TerrainChunk& GetChunkAt(const int xCoord, const int yCoord);

			// Generator used to generate new chunks
			const TerrainGenerator& mTerrainGenerator;
			//Terrain aesthetic used for rendering
			TerrainAesthetic mAesthetic = LOW_POLY;
			// Map of previously generated chunks
			std::map<std::pair<int, int>, TerrainChunk*> mChunkMap = std::map<std::pair<int, int>, TerrainChunk*>();
		};
	}
}
