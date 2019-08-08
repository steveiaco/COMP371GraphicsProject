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
			float crrtAmplitude = mAmplitude;
			float crrtFrequency = mFrequency;

			float maxHeight = 0.f;
			float minHeight = 0.f;

			for (int i = 0; i <= TerrainChunk::CHUNK_SIZE; i++)
			{
				for (int j = 0; j <= TerrainChunk::CHUNK_SIZE; j++)
				{
					chunk.mHeightMap[i][j] = crrtAmplitude * mNoise.Perlin(crrtFrequency * static_cast <float> (chunk.mXCoord + i), crrtFrequency *  static_cast <float> (chunk.mYCoord + j));
				}
			}
			crrtAmplitude *= mPersistence;
			crrtFrequency *= mLacunarity;

			for (int octave = 1; octave < mNumOctaves - 1; octave++)
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

			for (int i = 0; i < TerrainChunk::CHUNK_SIZE; i++)
			{
				for (int j = 0; j < TerrainChunk::CHUNK_SIZE; j++)
				{
					chunk.mHeightMap[i][j] += crrtAmplitude * mNoise.Perlin(crrtFrequency * static_cast <float> (chunk.mXCoord + i), crrtFrequency *  static_cast <float> (chunk.mYCoord + j));
					maxHeight = (maxHeight > chunk.mHeightMap[i][j]) ? maxHeight : chunk.mHeightMap[i][j];
					minHeight = (minHeight < chunk.mHeightMap[i][j]) ? minHeight : chunk.mHeightMap[i][j];
				}
			}

			for (int i = 0; i < TerrainChunk::CHUNK_SIZE; i++)
			{
				for (int j = 0; j < TerrainChunk::CHUNK_SIZE; j++)
				{
					chunk.mHeightMap[i][j] = Ease((chunk.mHeightMap[i][j] - minHeight) / (maxHeight - minHeight)) * (maxHeight - minHeight) + minHeight;
				}
			}
		}

		float TerrainGenerator::GetHumidityAt(const int mXCoord, const int mYCoord) const 
		{ 
			return mNoise.Perlin(mHumidityFrequency * (HUMIDITY_OFFSET_X + mXCoord), mHumidityFrequency * (HUMIDITY_OFFSET_Y + mYCoord)); 
		}

		float TerrainGenerator::GetTemperatureAt(const int mXCoord, const int mYCoord) const
		{ 
			return mNoise.Perlin(mTemperatureFrequency * (TEMPERATURE_OFFSET_X + mXCoord), mTemperatureFrequency * (TEMPERATURE_OFFSET_Y + mYCoord)); 
		}
	}
}