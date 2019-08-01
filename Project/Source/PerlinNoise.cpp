#include "PerlinNoise.h"

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <cmath>

// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]
float lerp(float a0, float a1, float w) 
{
	return (1.0f - w)*a0 + w * a1;
}

PerlinNoise::PerlinNoise()
{
	for (int index = 0; index < SIZE; ++index) 
	{
		int other = rand() % (index + 1);
		if (index > other)
		{
			mHash[index] = mHash[other];
		}
		mHash[other] = index;
		mGradient[index * 2    ] = cosf(2.0f * M_PI * index / SIZE);
		mGradient[index * 2 + 1] = sinf(2.0f * M_PI * index / SIZE);
	}
}

PerlinNoise::~PerlinNoise()
{
}

// Computes the dot product of the distance and gradient vectors.
float PerlinNoise::DotGridGradient(unsigned int ix, unsigned int iy, float x, float y) 
{
	// Compute the distance vector
	float dx = x - ix;
	float dy = y - iy;

	//Get gradient vector index
	int hashValue = mHash[(mHash[ix & MASK] + iy) & MASK];

	// Compute the dot-product
	return (dx * mGradient[hashValue] + dy * mGradient[hashValue + 1]);
}

// Compute Perlin noise at coordinates x, y
float PerlinNoise::Perlin(float x, float y) 
{
	// Determine grid cell coordinates
	int x0 = static_cast<int> (x);
	int x1 = x0 + 1;
	int y0 = static_cast<int> (y);
	int y1 = y0 + 1;

	// Determine interpolation weights
	float tx = x - static_cast<float> (x0);
	float sx = tx * tx * tx * (tx * (tx * 6 - 15) + 10);
	float ty = y - static_cast<float> (y0);
	float sy = ty * ty * ty * (ty * (ty * 6 - 15) + 10);

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, value;

	n0 = DotGridGradient(x0, y0, x, y);
	n1 = DotGridGradient(x1, y0, x, y);
	ix0 = lerp(n0, n1, sx);

	n0 = DotGridGradient(x0, y1, x, y);
	n1 = DotGridGradient(x1, y1, x, y);
	ix1 = lerp(n0, n1, sx);

	//Range of perlin noise ISN'T -1 to 1 (I learned this the hard way). It's actually -sqrt(0.5) to sqrt(0.5). We will map it to -1 to 1 though.
	value = lerp(ix0, ix1, sy);
	value = value / sqrt(0.5);
	return value;
}