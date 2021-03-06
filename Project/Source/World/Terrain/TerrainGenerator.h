// COMP 371 Assignment Framework
//
// Created by Emanuel Sharma
// Generates the chunks used to populate terrain
//
#pragma once

#include <vector>
#include <glm/glm.hpp>

class PerlinNoise;

namespace pg
{
	namespace terrain
	{
		class TerrainChunk;
		class Terrain;

		class TerrainGenerator
		{
		public:
			TerrainGenerator() = delete;
			TerrainGenerator(const PerlinNoise& noise);
			TerrainGenerator(const TerrainGenerator& orig);

			~TerrainGenerator();
			//WARNING: This code took a lot of effort to put together, but it is not complete. It is netiher optimized, nor cleaned up. Also, it does not work very well on this branch. If you want to test it, you are better off using the version included in the erosion branch, howver, note that even that verison is not complete.
			//	Inspired by https://www.firespark.de/resources/downloads/implementation%20of%20a%20methode%20for%20hydraulic%20erosion.pdf and https://github.com/SebLague/Hydraulic-Erosion
			void Erode(Terrain& terrain, const float minXCoord, const float minYCoord, const float maxXCoord, const float maxYCoord) const;

			// Get humidity at coordinates (used to determine biome)
			float GetHumidityAt(const float mXCoord, const float mYCoord) const;
			// Get temperature at coordinates (used to determine biome)
			float GetTemperatureAt(const float mXCoord, const float mYCoord) const;
			// Generate Chunk with origin at (mXCoord, mYCoord)
			void FillChunk(TerrainChunk& chunk) const;
			// Color chunk vertices according to their normals, temperature, and humidity (must set normals first using UpdateNormals())
			void ColorChunk(TerrainChunk& chunk) const;

		private:
			// This is the key to everything. The same noise generator will be used to generate terrain height, humidity, and temperature.
			const PerlinNoise& mNoise;

			// To ensure that values for the three are not identical (we don't want all mountains to be humid, all hot areas to be lowlands, etc.), we will use an offset of (0,0) for height and two different offsets for humidity and temperature.
			const static int HUMIDITY_OFFSET_X = 50;
			const static int HUMIDITY_OFFSET_Y = 50;
			const static int TEMPERATURE_OFFSET_X = 150;
			const static int TEMPERATURE_OFFSET_Y = 150;
			const static int FLATNESS_OFFSET_X = 50;
			const static int FLATNESS_OFFSET_Y = 50;

			// Controls rate of change of temperature and humidity
			float mHumidityFrequency = 1.f / 1024.f;
			float mTemperatureFrequency = 1.f / 2048.f;

			// Height parameters
			unsigned short mNumOctaves = 6;
			float mFrequency = 1.f/128.f;
			float mLacunarity = 3.f;
			float mAmplitude = 140.f;
			float mPersistence = 0.3f;
			unsigned int mFlatnessDegree = 3;
			float mFlatnessFrequency = 1.f / 512.f;

			// Terrain Color parameters
			// DESERT
			glm::vec3 colorHotDryFlat = glm::vec3(0.9f, 0.6f, 0.3f);
			glm::vec3 colorHotDrySteep = glm::vec3(0.5f, 0.3f, 0.15f);
			// RAINFOREST
			glm::vec3 colorHotHumidFlat = glm::vec3(0.15f, 0.40f, 0.13f);
			glm::vec3 colorHotHumidSteep = glm::vec3(0.27f, 0.27f, 0.27f);
			// POLAR
			glm::vec3 colorColdDryFlat = glm::vec3(0.85f, 0.85f, 0.85f);
			glm::vec3 colorColdDrySteep = glm::vec3(0.5f, 0.5f, 0.5f);
			// FOREST
			glm::vec3 colorColdHumidFlat = glm::vec3(0.92f, 1.f, 0.48f);
			glm::vec3 colorColdHumidSteep = glm::vec3(0.85f, 0.56f, 0.46f);

			// Interpolating between biomes in two dimensions the way it is done in real life is hard (would require either really complicated formulas or the ability to draw the distribution to a texture and sample it in code)
			// To make things simple, we will use bilinear interpolation between 4 different biomes (one for every temp-humidity extreme). This means that we will have desert neighborign polar regions, but it will have to do given the time requirements.
			// SLOPE
			float maxSlope = 0.99f;
			float minSlope = 0.90f;
			// HUMIDITY
			float maxHumidity = 0.8f;
			float minHumidity = 0.6f;
			// TEMPERATURE
			float maxTemperature = 0.6f;
			float minTemperature = 0.4f;
		};
	}
}
