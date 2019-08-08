#pragma once

class PerlinNoise {
public:
	PerlinNoise();
	~PerlinNoise();

	float Perlin(float x, float y) const;

private:
	float DotGridGradient(unsigned int ix, unsigned int iy, float dx, float dy) const;

	//Number of gradients in the mGradient array. Higher values will be more memory intensive but allow for less regularity. Value should also be a power of 2 so that we can benefit from bit masking optimizations.
	static const unsigned int SIZE = 256;
	static const unsigned int MASK = SIZE-1;
	//This array will be used to look-up gradient vectors
	unsigned int mHash[SIZE];
	//Array of gradient vectors. This array will be calulcated at construction of the PerlinNoise object and referenced using a hash function. Such an implementation is more memory efficient at the cost of repeating and being less intuitive.
	float mGradient[SIZE*2];
};
