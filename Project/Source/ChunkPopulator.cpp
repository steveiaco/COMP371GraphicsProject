#include "ChunkPopulator.h"

namespace pg
{
	namespace terrain 
	{

		ChunkPopulator::ChunkPopulator(const TerrainGenerator & t, const PerlinNoise & p) : mTerrainGenerator(t), mNoise(p) { }
		
		void ChunkPopulator::AddObject(ChunkObject * c)
		{
			chunkObjects.push_back(c);
		}
		
		void ChunkPopulator::PopulateChunk(TerrainChunk * t)
		{
			for (std::vector<ChunkObject*>::iterator it = chunkObjects.begin(); it < chunkObjects.end(); it++) 
			{
				for (int x = 0; x < t->CHUNK_SIZE; x += STRIDE_X)
				{
					for (int y = 0; y < t->CHUNK_SIZE; y += STRIDE_Y) 
					{
						//First, we verify whether or not the object can be placed in position's biome
						if (VerifyObjectPlacement(it*, x, y)) {

						}
					}
				}
			}
		}
		
		bool ChunkPopulator::VerifyObjectPlacement(ChunkObject * o, const float x, const float y)
		{
			glm::vec2 humidity = o->GetHumidityRange();
			glm::vec2 temperature = o->GetTemperatureRange();
			glm::vec2 altitude = o->GetAltitudeRange();

			bool humidityIsAcceptable = mTerrain.GetHumidityAt(x, y) > humidity.x && mTerrain.GetHumidityAt(x, y) < humidity.y;
			bool temperatureIsAcceptable = mTerrain.GetTemperatureAt(x, y) > temperature.x && mTerrain.GetTemperatureAt(x, y) < temperature.y;
			bool altitudeIsAcceptable = mTerrain.GetHeightAt(x, y) > altitude.x && mTerrain.GetHeightAt(x, y) < altitude.y;

			return humidityIsAcceptable && temperatureIsAcceptable && altitudeIsAcceptable;
		}
	}
}

