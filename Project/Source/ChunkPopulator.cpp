#include "ChunkPopulator.h"
#include "PerlinNoise.h"

namespace pg
{
	namespace terrain 
	{

		ChunkPopulator::ChunkPopulator(const Terrain & t, const PerlinNoise & p) : mTerrain(t), mNoise(p) { }
		
		void ChunkPopulator::AddObject(ChunkObject * c)
		{
			chunkObjects.push_back(c);
		}
		
		void ChunkPopulator::PopulateChunk(TerrainChunk * t)
		{
			for (std::vector<ChunkObject*>::iterator it = chunkObjects.begin(); it < chunkObjects.end(); it++) 
			{
				ChunkObject* co = *it;

				float chunkOriginX = t->mXCoord;
				float chunkOriginY = t->mYCoord;

				for (float x = chunkOriginX; x < t->CHUNK_SIZE + chunkOriginX - 1; x += STRIDE_X)
				{
					for (float y = chunkOriginY; y < t->CHUNK_SIZE + chunkOriginY - 1; y += STRIDE_Y)
					{
						//First, we verify whether or not the object can be placed in position's biome
						if (VerifyObjectPlacement(*it, x, y)) 
						{
							//Then we verify whether the perlin noise allows us to place it in this location
							float perl = mNoise.Perlin(x, y);
							if (perl > 0) 
							{
								//float xWorld = x + 
								//float yWorld = y + 

								//Then, if perlin allows for it, we will randomly decide whether the object will spawn
								if (rand() % 100 <= 35) {
									glm::vec3 position = glm::vec3(x, y, mTerrain.GetHeightAt(x, y));

									glm::vec3 maxS = co->GetMaxScaling();
									glm::vec3 minS = co->GetMinScaling();
									//todo fix this casting
									glm::vec3 scaling = glm::vec3(rand() % (int)maxS.x + minS.x, rand() % (int)maxS.x + minS.x, rand() % (int)maxS.x + minS.x);

									glm::vec3 maxRA = co->GetMaxRotationAngle();
									glm::vec3 minRA = co->GetMinRotationAngle();
									//this generates rotation angles for the x,y,z axis
									//todo fix this casting
									glm::vec3 rotation = glm::vec3(rand() % (int)maxRA.x + minRA.x, rand() % (int)maxRA.y + minRA.y, rand() % (int)maxRA.z + minRA.z);

									ChunkObjectInstance* coi = new ChunkObjectInstance(co);
									coi->SetPosition(position);
									coi->SetRotation(rotation);
									coi->SetScaling(scaling);

									t->AddObjectInstance(coi);
								}
							}
						}
					}
				}
			}


			//Order the object instances by type to optimize drawing
			t->SortObjectInstances();

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

