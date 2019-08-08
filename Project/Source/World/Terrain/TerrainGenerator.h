#pragma once

#include <vector>

class PerlinNoise;

namespace pg
{
	namespace terrain
	{
		class TerrainChunk;

		class TerrainGenerator
		{
		public:
			TerrainGenerator() = delete;
			TerrainGenerator(const PerlinNoise& noise);
			TerrainGenerator(const TerrainGenerator& orig);

			~TerrainGenerator();

			// Get humidity at coordinates (used to determine biome)
			float GetHumidityAt(const int mXCoord, const int mYCoord) const;
			// Get temperature at coordinates (used to determine biome)
			float GetTemperatureAt(const int mXCoord, const int mYCoord) const;

			// Generate Chunk with origin at (mXCoord, mYCoord)
			void FillChunk(TerrainChunk& chunk) const;

		private:
			// This is the key to everything. The same noise generator will be used to generate terrain height, humidity, and temperature.
			const PerlinNoise& mNoise;
			// To ensure that values for the three are not identical (we don't want all mountains to be humid, all hot areas to be lowlands, etc.), we will use an offset of (0,0) for height and two different offsets for humidity and temperature.
			const static int HUMIDITY_OFFSET_X = 50;
			const static int HUMIDITY_OFFSET_Y = 50;
			const static int TEMPERATURE_OFFSET_X = 150;
			const static int TEMPERATURE_OFFSET_Y = 150;

			// Controls rate of change of temperature and humidity
			float mHumidityFrequency = 1.f / 256.f;
			float mTemperatureFrequency = 1.f / 512.f;

			// Height data
			unsigned short mNumOctaves = 5;
			float mFrequency = 1.f/128.f;
			float mLacunarity = 3.f;
			float mAmplitude = 20.f;
			float mPersistence = 0.4f;
		};
	}
}
