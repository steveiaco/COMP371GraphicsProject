// COMP 371 Assignment Framework
//
// Created by Emanuel Sharma
// Generates the chunks used to populate terrain
//
#include "TerrainGenerator.h"
#include "TerrainChunk.h"
#include "Terrain.h"
#if defined(GLM_PLATFORM_APPLE) || defined(GLM_PLATFORM_LINUX)
#include "../../PerlinNoise.h"
#else
#include "..\..\PerlinNoise.h"
#endif

namespace pg
{
	namespace terrain
	{
		TerrainGenerator::TerrainGenerator(const PerlinNoise& noise)
			: mNoise(noise)
		{
		}

		TerrainGenerator::TerrainGenerator(const TerrainGenerator& orig)
			: mNoise(orig.mNoise)
		{
		}

		TerrainGenerator::~TerrainGenerator()
		{
		}
		
		// Helper function (use this to yield sharper terrain features)
		float Ease(float t)
		{
			const float exp = 3.f;
			return (t < 0.5f) ? 0.5f * glm::pow(2.f * t, exp) : 0.5f * glm::pow(2.f * t - 2.f, exp) + 1.f;
		}

		void TerrainGenerator::FillChunk(TerrainChunk& chunk) const
		{
			// Feel free to play with this method to get the kind of terrain that you want!

			// Initialize attributes
			float crrtAmplitude = mAmplitude;
			float crrtFrequency = mFrequency;
			float flatness[TerrainChunk::CHUNK_SIZE + 2][TerrainChunk::CHUNK_SIZE + 2];

			//Calculate flatness for all vertices once
			for (int i = 0; i < TerrainChunk::CHUNK_SIZE + 2; i++)
			{
				for (int j = 0; j < TerrainChunk::CHUNK_SIZE + 2; j++)
				{
					flatness[i][j] = glm::pow(0.5f * (1.f + mNoise.Perlin(static_cast <float> (chunk.mXCoord + i) * mFlatnessFrequency, static_cast <float> (chunk.mYCoord + j) * mFlatnessFrequency)), mFlatnessDegree);
				}
			}

			//Set initial terrain height to height of first layer of noise
			for (int i = 0; i < TerrainChunk::CHUNK_SIZE + 2; i++)
			{
				for (int j = 0; j < TerrainChunk::CHUNK_SIZE + 2; j++)
				{
					chunk.mHeightMap[i][j] = flatness[i][j] * crrtAmplitude * mNoise.Perlin(crrtFrequency * static_cast <float> (chunk.mXCoord + i), crrtFrequency *  static_cast <float> (chunk.mYCoord + j));
					// We will add once layer of additional noise to make terrain a little more rough (makes the low_poly aesthetic shine)
					chunk.mHeightMap[i][j] += flatness[i][j] * 3.f * mNoise.Perlin(static_cast <float> (chunk.mXCoord + i) / 2.f, static_cast <float> (chunk.mYCoord + j) / 2.f);
				}
			}
			crrtAmplitude *= mPersistence;
			crrtFrequency *= mLacunarity;

			//Add additional layers of noise
			for (int octave = 1; octave < mNumOctaves; octave++)
			{
				for (int i = 0; i < TerrainChunk::CHUNK_SIZE + 2; i++)
				{
					for (int j = 0; j < TerrainChunk::CHUNK_SIZE + 2; j++)
					{
						chunk.mHeightMap[i][j] += flatness[i][j] * crrtAmplitude * mNoise.Perlin(crrtFrequency * static_cast <float> (chunk.mXCoord + i), crrtFrequency *  static_cast <float> (chunk.mYCoord + j));
					}
				}
				crrtAmplitude *= mPersistence;
				crrtFrequency *= mLacunarity;
			}
		}

		void TerrainGenerator::ColorChunk(TerrainChunk& chunk) const
		{
			for (int i = 0; i <= TerrainChunk::CHUNK_SIZE; i++)
			{
				for (int j = 0; j <= TerrainChunk::CHUNK_SIZE; j++)
				{
					// Get temperature, humidity, and steepness (we will clamp so that we have a range of consistent colors in addition to a range of interpolated colors)
					// TEMP
					float temperature = GetTemperatureAt(static_cast <float> (chunk.mXCoord + i), static_cast <float> (chunk.mYCoord + j));
					temperature = (temperature - minTemperature) / (maxTemperature - minTemperature);
					temperature = glm::clamp(temperature, 0.0f, 1.0f);
					// HUMIDITY
					float humidity = GetHumidityAt(static_cast <float> (chunk.mXCoord + i), static_cast <float> (chunk.mYCoord + j));
					humidity = (humidity - minHumidity) / (maxHumidity - minHumidity);
					humidity = glm::clamp(humidity, 0.0f, 1.0f);
					// SLOPE
					//Uncomment the code below for linear interpolation of slope. I switched to an exponential distribution instead because I find the linear one boring to look at.
					float slope = glm::dot(chunk.mNormalMap[i][j], glm::vec3(0.f, 1.f, 0.f));
					slope = glm::pow(slope, 5.f);
					//slope = (slope - minSlope) / (maxSlope - minSlope);
					//slope = glm::clamp(slope, 0.0f, 1.0f);

					// Using trilinear interpolation, get interpolated color, one dimension at a time:
					// SLOPE
					glm::vec3 coldHumid = glm::mix(colorColdHumidSteep, colorColdHumidFlat, slope);
					glm::vec3 coldDry = glm::mix(colorColdDrySteep, colorColdDryFlat, slope);
					glm::vec3 hotHumid = glm::mix(colorHotHumidSteep, colorHotHumidFlat, slope);
					glm::vec3 hotDry = glm::mix(colorHotDrySteep, colorHotDryFlat, slope);
					// HUMIDITY
					glm::vec3 cold = glm::mix(coldDry, coldHumid, humidity);
					glm::vec3 hot = glm::mix(hotDry, hotHumid, humidity);
					// TEMPERATURE
					chunk.mColorMap[i][j] = glm::mix(cold, hot, temperature);
				}
			}
		}

		float TerrainGenerator::GetHumidityAt(const float mXCoord, const float mYCoord) const 
		{ 
			return 0.5f + 0.5f * mNoise.Perlin(mHumidityFrequency * (HUMIDITY_OFFSET_X + mXCoord), mHumidityFrequency * (HUMIDITY_OFFSET_Y + mYCoord));
		}

		float TerrainGenerator::GetTemperatureAt(const float mXCoord, const float mYCoord) const
		{ 
			return 0.5f + 0.5f * mNoise.Perlin(mTemperatureFrequency * (TEMPERATURE_OFFSET_X + mXCoord), mTemperatureFrequency * (TEMPERATURE_OFFSET_Y + mYCoord)); 
		}

		//WARNING: This code took a lot of effort to put together, but it is not complete. It is netiher optimized, nor cleaned up. Also, it does not work very well on this branch. If you want to test it, you are better off using the version included in the erosion branch, howver, note that even that verison is not complete.
		//	Inspired by https://www.firespark.de/resources/downloads/implementation%20of%20a%20methode%20for%20hydraulic%20erosion.pdf
		void TerrainGenerator::Erode(Terrain& terrain, const float minXCoord, const float minYCoord, const float maxXCoord, const float maxYCoord) const
		{
			const float inertia = .05f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
			const float sedimentCapacityFactor = 5.f; // Multiplier for how much sediment a droplet can carry
			const float minSedimentCapacity = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
			constexpr float erosionRadius = 1.f;
			const float erodeSpeed = .5f;
			const float depositSpeed = .3f;
			const float evaporateSpeed = .01f;
			const float gravity = 4.f;
			const int maxDropletLifetime = 30;

			//Iterate over droplet simulations
			for (int i = 0; i < 150000; i++)
			{
				//Set initial droplet parameters
				glm::vec2 pos	(		
									(maxXCoord - minXCoord) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + minXCoord,
									(maxYCoord - minYCoord) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + minYCoord
								);
				glm::vec2 dir(0.f, 0.f);
				float speed = 1.f;
				float water = 1.f;
				float sediment = 1.f;

				//Droplet lifetime is measured in steps
				for (int n = 0; n < maxDropletLifetime; n++)
				{
					//Get coordinates
					int floorX = static_cast <int> (glm::floor(pos.x));
					int floorY = static_cast <int> (glm::floor(pos.y));
					float dx = pos.x - static_cast <float> (floorX);
					float dy = pos.y - static_cast <float> (floorY);

					//Get height of corners
					float heightNE = terrain.GetHeightAt(floorX + 1, floorY);
					float heightNW = terrain.GetHeightAt(floorX, floorY);
					float heightSW = terrain.GetHeightAt(floorX, floorY + 1);
					float heightSE = terrain.GetHeightAt(floorX + 1, floorY + 1);

					//Interpolate height of droplet and gradient of height map (We will use the four corners of the square instead of the triangles)
					float height = (heightNW * (1 - dx) + heightNE * dx) * (1 - dy) + (heightSW * (1 - dx) + heightSE * dx) * dy;
					glm::vec2 gradient{ (heightNE - heightNW) * (1 - dy) + (heightSE - heightSW) * dy,
										(heightSW - heightNW) * (1 - dx) + (heightSE - heightNE) * dx };

					// Update direction and position of water droplet (direction will be old direction under influence of gradient according to inertia)
					dir = dir * inertia - gradient * (1.f - inertia);
					// If direction is null, give random direction (alternative is to break)
					if (dir.x == 0.f && dir.y == 0.f)
					{
						dir.x = rand() - RAND_MAX / 2;
						dir.y = rand() - RAND_MAX / 2;
					}
					dir = glm::normalize(dir);
					pos += dir;

					//Stop simulation if water droplet has gone off of map
					if (pos.x < minXCoord || pos.y < minYCoord || pos.x > maxXCoord || pos.y > maxYCoord)
					{
						break;
					}

					//Get new coordinates
					int newFloorX = static_cast <int> (glm::floor(pos.x));
					int newFloorY = static_cast <int> (glm::floor(pos.y));
					int newDX = pos.x - newFloorX;
					int newDY = pos.y - newFloorY;

					//Get height of new corners
					float newHeightNE = terrain.GetHeightAt(newFloorX + 1, newFloorY);
					float newHeightNW = terrain.GetHeightAt(newFloorX, newFloorY);
					float newHeightSW = terrain.GetHeightAt(newFloorX, newFloorY + 1);
					float newHeightSE = terrain.GetHeightAt(newFloorX + 1, newFloorY + 1);

					//Interpolate height of droplet (We will use the four corners of the square instead of the triangles)
					float newHeight = (newHeightNW * (1 - newDX) + newHeightNE * newDX) * (1 - newDY) + (newHeightSW * (1 - newDX) + newHeightSE * newDX) * newDY;
					float dh = newHeight - height;

					// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
					float sedimentCapacity = glm::max(-dh * speed * water * sedimentCapacityFactor, minSedimentCapacity);

					// If carrying more sediment than capacity, or if flowing uphill, deposit sediment:
					if (sediment > sedimentCapacity || dh > 0) {
						// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
						float amountToDeposit = (dh > 0.f) ? glm::min(dh, sediment) : (sediment - sedimentCapacity) * depositSpeed;
						sediment -= amountToDeposit;

						// Add the sediment to the four nodes of the current cell using bilinear interpolation
						terrain.SetHeightAt(floorX, floorY, heightNW + amountToDeposit * (1 - dx) * (1 - dy));
						terrain.SetHeightAt(floorX + 1, floorY, heightNE + amountToDeposit * dx * (1 - dy));
						terrain.SetHeightAt(floorX, floorY + 1, heightSW + amountToDeposit * (1 - dx) * dy);
						terrain.SetHeightAt(floorX + 1, floorY + 1, heightSE + amountToDeposit * dx * dy);
					}
					else {
						// Erode a fraction of the droplet's current carry capacity.
						// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
						float amountToErode = glm::min(-dh, (sedimentCapacity - sediment) * erodeSpeed);
						if (amountToErode == 0.f)
							continue;

						////Get boundaries
						int left = glm::max(static_cast <int> (glm::ceil(pos.x - erosionRadius)), 0);
						int right = glm::min(static_cast <int> (glm::floor(pos.x + erosionRadius)), static_cast <int> (maxXCoord));
						int top = glm::max(static_cast <int> (glm::ceil(pos.y - erosionRadius)), 0);
						int bottom = glm::min(static_cast <int> (glm::floor(pos.y + erosionRadius)), static_cast <int> (maxXCoord));

						float* nodes[4 * static_cast<int> (erosionRadius) * static_cast<int> (erosionRadius)];
						float weight[4 * static_cast<int> (erosionRadius) * static_cast<int> (erosionRadius)];
						float weightSum = 0.f;
						int count = 0;

						//Iterate over nodes within boundaries and add those which are within radius to list
						for (int x = left; x <= right; x++)
						{
							for (int y = top; y <= bottom; y++)
							{
								//If node is within radius, erode it
								float distance = glm::length(glm::vec2(x, y) - pos);
								if (distance <= erosionRadius)
								{
									float weightedAmountToErode = amountToErode * (1 - distance / erosionRadius);
									nodes[count] = terrain.GetHeightRefAt(x, y);
									weight[count] = weightedAmountToErode;
									count++;
									weightSum += weightedAmountToErode;
								}
							}
						}

						//Iterate over nodes within list and erode them
						for (int index = 0; index < count; index++)
						{
							float weighedErodeAmount = amountToErode * weight[index] / weightSum;
							float heighthere = *nodes[index];
							float deltaSediment = (*nodes[index] < weighedErodeAmount) ? *nodes[index] : weighedErodeAmount;
							*nodes[index] -= deltaSediment;
							sediment += deltaSediment;
						}
					}

					// Update droplet's speed and water content
					speed = glm::sqrt(glm::max(speed * speed + dh * gravity, 0.f));
					water *= (1 - evaporateSpeed);
				}
			}
		}
	}
}