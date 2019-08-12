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

			float chunkOriginX = t->mXCoord;
			float chunkOriginY = t->mYCoord;

			for (float x = chunkOriginX; x < t->CHUNK_SIZE + chunkOriginX - 1; x += STRIDE_X)
			{
				for (float y = chunkOriginY; y < t->CHUNK_SIZE + chunkOriginY - 1; y += STRIDE_Y)
				{
					for (std::vector<ChunkObject*>::iterator it = chunkObjects.begin(); it < chunkObjects.end(); it++)
					{
						ChunkObject* co = *it;

						//Then, if perlin allows for it, we will randomly decide whether the object will spawn
						float randSpawn = rand() / (float)RAND_MAX;
						if (randSpawn <= co->GetDensity() / 100)
						{
							//First, we verify whether or not the object can be placed in position's biome
							if (VerifyObjectPlacement(*it, x, y))
							{
								//Then we verify whether the perlin noise allows us to place it in this location
								//float perl = mNoise.Perlin(x * FREQUENCY_PERLIN, y * FREQUENCY_PERLIN);
								if (true)
								{
									float terrainHeight = mTerrain.GetHeightAt(x, y);

									/* Position */
									glm::vec3 position = glm::vec3(x, terrainHeight, y);


									/* Scaling */
									glm::vec3 maxS = co->GetMaxScaling();
									glm::vec3 minS = co->GetMinScaling();

									glm::vec3 scaling;

									//if the scaling factors are uniform, then use the same scaling factor for XYZ
									if (maxS.x == maxS.y && maxS.x == maxS.z && minS.x == minS.y && minS.x == minS.z)
									{
										float scaleRandXYZ = rand() / (float)RAND_MAX;
										float scale = scaleRandXYZ * (maxS.x - minS.x) + minS.x;
										scaling = glm::vec3(scale, scale, scale);
									}
									else
									{
										float scaleRandX = rand() / (float)RAND_MAX;
										float scaleRandY = rand() / (float)RAND_MAX;
										float scaleRandZ = rand() / (float)RAND_MAX;

										scaling = glm::vec3(scaleRandX * (maxS.x - minS.x) + minS.x, scaleRandY * (maxS.y - minS.y) + minS.y, scaleRandZ * (maxS.z - minS.z) + minS.z);
									}


									/* Rotation */
									glm::vec3 maxRA = co->GetMaxRotationAngle();
									glm::vec3 minRA = co->GetMinRotationAngle();
									//this generates rotation angles for the x,y,z axis
									//todo fix this casting

									//generate a number between [0,1]
									float randRotationX = rand() / (float)RAND_MAX;
									float randRotationY = rand() / (float)RAND_MAX;
									float randRotationZ = rand() / (float)RAND_MAX;

									glm::vec3 rotation = glm::vec3(randRotationX * (maxRA.x - minRA.x) + minRA.x, randRotationY * (maxRA.y - minRA.y) + minRA.y, randRotationZ * (maxRA.z - minRA.z) + minRA.z);

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


			float humidityRandom = rand() / (float)RAND_MAX;
			float temperatureRandom = rand() / (float)RAND_MAX;
			float altitudeRandom = rand() / (float)RAND_MAX;

			//quadratic function used to make objects more likely to spawn in the center of a range, producing a gradual transition between biomes.
			float quadFunctionHumidity = 1 - std::pow( ( 2 * (mTerrain.GetHumidityAt(x, y) - (humidity.y + humidity.x) / 2 ) ) / (humidity.y - humidity.x) , 2);
			float quadFunctionTemperature = 1 - std::pow( ( 2 * (mTerrain.GetTemperatureAt(x, y) - (temperature.y + temperature.x) / 2 ) ) / (temperature.y - temperature.x) , 2);

			bool humidityIsAcceptable = humidityRandom < glm::clamp(quadFunctionHumidity, 0.0f, 1.0f);
			bool temperatureIsAcceptable = temperatureRandom < glm::clamp(quadFunctionTemperature, 0.0f, 1.0f);
			bool altitudeIsAcceptable = mTerrain.GetHeightAt(x, y) > altitude.x && mTerrain.GetHeightAt(x, y) < altitude.y;
						
			return humidityIsAcceptable && temperatureIsAcceptable && altitudeIsAcceptable;
		}
	}
}

