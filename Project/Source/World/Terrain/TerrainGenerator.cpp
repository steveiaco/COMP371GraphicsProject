#include "TerrainGenerator.h"
#include "TerrainChunk.h"
#include "..\..\PerlinNoise.h"

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
		
		float Ease(float t)
		{
			const float exp = 3.f;
			return (t < 0.5f) ? 0.5f * glm::pow(2.f * t, exp) : 0.5f * glm::pow(2.f * t - 2.f, exp) + 1.f;
		}

		void TerrainGenerator::FillChunk(TerrainChunk& chunk) const
		{
			static float test = mNoise.Perlin(10.f, 10.f);
			float newTest = mNoise.Perlin(10.f, 10.f);
			assert(newTest == test);

			float crrtAmplitude = mAmplitude;
			float crrtFrequency = mFrequency;

			float maxHeight = 0.f;
			float minHeight = 0.f;

			for (int i = 0; i <= TerrainChunk::CHUNK_SIZE; i++)
			{
				for (int j = 0; j <= TerrainChunk::CHUNK_SIZE; j++)
				{
					chunk.mHeightMap[i][j] = crrtAmplitude * Ease(mNoise.Perlin(crrtFrequency * static_cast <float> (chunk.mXCoord + i), crrtFrequency *  static_cast <float> (chunk.mYCoord + j)));
				}
			}
			crrtAmplitude *= mPersistence;
			crrtFrequency *= mLacunarity;

			for (int octave = 1; octave < mNumOctaves; octave++)
			{
				for (int i = 0; i <= TerrainChunk::CHUNK_SIZE; i++)
				{
					for (int j = 0; j <= TerrainChunk::CHUNK_SIZE; j++)
					{
						chunk.mHeightMap[i][j] += crrtAmplitude * mNoise.Perlin(crrtFrequency * static_cast <float> (chunk.mXCoord + i), crrtFrequency *  static_cast <float> (chunk.mYCoord + j));
					}
				}
				crrtAmplitude *= mPersistence;
				crrtFrequency *= mLacunarity;
			}

			chunk.UpdateNormals();
			ColorChunk(chunk);
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

		float TerrainGenerator::GetHumidityAt(const int mXCoord, const int mYCoord) const 
		{ 
			return 0.5f + 0.5f * mNoise.Perlin(mHumidityFrequency * (HUMIDITY_OFFSET_X + mXCoord), mHumidityFrequency * (HUMIDITY_OFFSET_Y + mYCoord));
		}

		float TerrainGenerator::GetTemperatureAt(const int mXCoord, const int mYCoord) const
		{ 
			return 0.5f + 0.5f * mNoise.Perlin(mTemperatureFrequency * (TEMPERATURE_OFFSET_X + mXCoord), mTemperatureFrequency * (TEMPERATURE_OFFSET_Y + mYCoord)); 
		}
	}
}