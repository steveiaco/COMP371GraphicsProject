#include "Terrain.h"
#include "Renderer.h"
#include "PerlinNoise.h"
#include "World.h"
#include "EventManager.h"
#include <GLFW/glfw3.h>
#include <iostream>

PerlinNoise Terrain::Chunk::noiseGenerator = PerlinNoise();

Terrain* t;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		t->Erode();

		for (int i = 0; i < t->mWidth; i++)
		{
			for (int j = 0; j < t->mHeight; j++)
			{
				// Free the GPU from the Vertex Buffer
				glDeleteBuffers(1, &t->mChunkMap[i][j]->mVBO);
				glDeleteVertexArrays(1, &t->mChunkMap[i][j]->mVAO);
				t->mChunkMap[i][j]->GenVertexBuffer();
			}
		}
	}
}

Terrain::Chunk::Chunk()
{
}

float Ease(float t)
{
	const float exp = 3.f;
	return t;
	//return (t < 0.5f) ? 0.5f * glm::pow(2.f * t, exp) : 0.5f * glm::pow(2.f * t - 2.f, exp) + 1.f;
}

Terrain::Chunk::Chunk(int xCoord, int yCoord)
{
	float amplitude = 50.f;
	float frequency = 1.f/150.f;

	unsigned int numOctaves = 5;
	float lacunarity = 3.f;
	float persistence = 0.25f;

	float maxHeight = 0.f;
	float minHeight = 0.f;

	mXCoord = xCoord;
	mYCoord = yCoord;

	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		float height = static_cast <float> (i) / (CHUNK_SIZE - 1);
		height = amplitude * glm::sin(height * 30.f) + amplitude;
		if (height < 2.f)
		{
			height = 5.f;
		}
		//std::cout << height << std::endl;
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			mHeightMap[i][j] = amplitude * Chunk::noiseGenerator.Perlin(frequency * static_cast <float> (mXCoord + i), frequency *  static_cast <float> (mYCoord + j));
		}
	}
	amplitude *= persistence;
	frequency *= lacunarity;

	for (int octave = 0; octave < numOctaves-1; octave++)
	{
		for (int i = 0; i < CHUNK_SIZE; i++)
		{
			for (int j = 0; j < CHUNK_SIZE; j++)
			{
				mHeightMap[i][j] += amplitude * Chunk::noiseGenerator.Perlin(frequency * static_cast <float> (mXCoord + i), frequency *  static_cast <float> (mYCoord + j));
			}
		}
		amplitude *= persistence;
		frequency *= lacunarity;
	}

	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			mHeightMap[i][j] += amplitude * Chunk::noiseGenerator.Perlin(frequency * static_cast <float> (mXCoord + i), frequency *  static_cast <float> (mYCoord + j));
			maxHeight = (maxHeight > mHeightMap[i][j]) ? maxHeight : mHeightMap[i][j];
			minHeight = (minHeight < mHeightMap[i][j]) ? minHeight : mHeightMap[i][j];
		}
	}

	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			mHeightMap[i][j] = Ease((mHeightMap[i][j] - minHeight) / (maxHeight - minHeight)) * (maxHeight - minHeight) + 10.f;
		}
	}
}

Terrain::Chunk::~Chunk()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void Terrain::Chunk::GenVertexBuffer()
{
	float scale = 0.5f;

	Vertex* vertexBufer = new Vertex[2 * (CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * 3];
	int crrtVertex = 0;

	for (int x = 0; x < CHUNK_SIZE - 1; x++)
	{
		for (int y = 0; y < CHUNK_SIZE - 1; y++)
		{
			glm::vec3 topLeft = scale * glm::vec3(mXCoord + x, mHeightMap[x][y], mYCoord + y);
			glm::vec3 bottomLeft = scale * glm::vec3(mXCoord + x, mHeightMap[x][y + 1], mYCoord + y + 1);
			glm::vec3 bottomRight = scale * glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][y + 1], mYCoord + y + 1);
			glm::vec3 topRight = scale * glm::vec3(mXCoord + x + 1, mHeightMap[x + 1][y], mYCoord + y);

			glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - topRight, bottomLeft - topRight));
			glm::vec3 normal2 = glm::normalize(glm::cross(bottomLeft - topRight, bottomRight - topRight));

			float verticality = glm::dot(normal1, glm::vec3(0.f, 1.f, 0.f));
			glm::vec3 color1 = glm::mix(glm::vec3(0.85f, 0.56f, 0.46f), glm::vec3(0.92f, 1.f, 0.48), glm::pow(verticality, 5.f));
			verticality = glm::dot(normal2, glm::vec3(0.f, 1.f, 0.f));
			glm::vec3 color2 = glm::mix(glm::vec3(0.85f, 0.56f, 0.46f), glm::vec3(0.92f, 1.f, 0.48f), glm::pow(verticality, 5.f));

			vertexBufer[crrtVertex++] = Vertex{ topRight, normal1, color1 };
			vertexBufer[crrtVertex++] = Vertex{ topLeft, normal1, color1 };
			vertexBufer[crrtVertex++] = Vertex{ bottomLeft, normal1, color1 };

			vertexBufer[crrtVertex++] = Vertex{ topRight, normal2, color2 };
			vertexBufer[crrtVertex++] = Vertex{ bottomLeft, normal2, color2 };
			vertexBufer[crrtVertex++] = Vertex{ bottomRight, normal2, color2 };
		}
	}

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, crrtVertex * sizeof(Vertex), vertexBufer, GL_STATIC_DRAW);

	// 1st attribute buffer : vertex Positions
	glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                // size
		GL_FLOAT,        // type
		GL_FALSE,        // normalized?
		sizeof(Vertex), // stride
		(void*)0        // array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normal
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)sizeof(glm::vec3)    // Normal is Offseted by vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(1);

	// 3rd attribute buffer : vertex color
	glVertexAttribPointer(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
	);
	glEnableVertexAttribArray(2);
}

void Terrain::Chunk::Draw()
{
	// Draw the Vertex Buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 worldMatrix(1.0f);
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

	// Draw the terrain
	glDrawArrays(GL_TRIANGLES, 0, 2 * (CHUNK_SIZE - 1) * (CHUNK_SIZE - 1) * 3);
}

Terrain::Terrain() : Terrain(1,1)
{
}

Terrain::Terrain(unsigned int width, unsigned int height) : mWidth(width), mHeight(height)
{
	t = this;
	glfwSetKeyCallback(EventManager::GetWindow(), key_callback);

	mChunkMap = new Chunk**[mWidth];
	for (int i = 0; i < mWidth; i++)
	{
		mChunkMap[i] = new Chunk*[mHeight];
		for (int j = 0; j < mHeight; j++)
		{
			mChunkMap[i][j] = new Chunk{ i*(Chunk::CHUNK_SIZE-1), j*(Chunk::CHUNK_SIZE-1) };
		}
	}

	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			mChunkMap[i][j]->GenVertexBuffer();
		}
	}
}

Terrain::~Terrain()
{
	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			delete mChunkMap[i][j];
		}
		delete [] mChunkMap[i];
	}
	delete [] mChunkMap;
}

void Terrain::Erode()
{
	const float inertia = .05f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
	const float sedimentCapacityFactor = 2.f; // Multiplier for how much sediment a droplet can carry
	const float minSedimentCapacity = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
	const float erosionRadius = 3.f;
	const float erodeSpeed = .3f;
	const float depositSpeed = .3f;
	const float evaporateSpeed = .01f;
	const float gravity = 4.f;
	const int maxDropletLifetime = 30;

	//Iterate over droplet simulations
	for (int i = 0; i < 1000000; i++)
	{
		//Set initial droplet parameters
		glm::vec2 pos(	static_cast <float> (mWidth * (Chunk::CHUNK_SIZE - 1)) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
						static_cast <float> (mHeight * (Chunk::CHUNK_SIZE - 1)) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		assert(0.f <= pos.x && pos.x <= mWidth * (Chunk::CHUNK_SIZE - 1));
		assert(0.f <= pos.y && pos.y <= mHeight * (Chunk::CHUNK_SIZE - 1));
		glm::vec2 dir(0.f, 0.f);
		float speed = 1.f;
		float water = 1.f;
		float sediment = 1.f;

		//Droplet lifetime is measured in steps
		for (int step = 0; step < maxDropletLifetime; step++)
		{
			//Get coordinates
			int floorX = static_cast <int> (glm::floor(pos.x));
			int floorY = static_cast <int> (glm::floor(pos.y));
			float dx = pos.x - static_cast <float> (floorX);
			float dy = pos.y - static_cast <float> (floorY);

			//Get height of corners
			float heightNE = GetHeightRef(floorX + 1, floorY);
			assert(heightNE == heightNE);
			float heightNW = GetHeightRef(floorX, floorY);
			float heightSW = GetHeightRef(floorX, floorY + 1);
			float heightSE = GetHeightRef(floorX + 1, floorY + 1);

			//Interpolate height of droplet and gradient of height map (We will use the four corners of the square instead of the triangles)
			float height = (heightNW * (1 - dx) + heightNE * dx) * (1 - dy) + (heightSW * (1 - dx) + heightSE * dx) * dy;
			glm::vec2 gradient{ (heightNE - heightNW) * (1 - dy) + (heightSE - heightSW) * dy, 
								(heightSW - heightNW) * (1 - dx) + (heightSE - heightNE) * dx };

			//if (step == 0 || step == maxDropletLifetime-1)
			//World::GetInstance()->AddSphere(glm::vec3(pos.x, height, pos.y), 0.5*water*water);

			//Update direction and position of water droplet (direction will be old direction under influence of gradient according to inertia)
			dir = dir * inertia - gradient * (1.f - inertia);
			if (dir.x == 0.f && dir.y == 0.f)
			{
				dir.x = rand() - RAND_MAX / 2;
				dir.y = rand() - RAND_MAX / 2;
			}
			dir = glm::normalize(dir);
			pos += dir;

			//Stop simulation if water droplet has gone off of map
			if (pos.x < 0.f || pos.y < 0.f || pos.x > (Chunk::CHUNK_SIZE - 1) * mWidth || pos.y >(Chunk::CHUNK_SIZE - 1) * mHeight)
			{
				break;
			}

			//Get new coordinates
			int newFloorX = static_cast <int> (glm::floor(pos.x));
			int newFloorY = static_cast <int> (glm::floor(pos.y));
			int newDX = pos.x - newFloorX;
			int newDY = pos.y - newFloorY;

			//Get height of new corners
			float newHeightNE = GetHeightRef(newFloorX + 1, newFloorY);
			assert(newHeightNE == newHeightNE);
			float newHeightNW = GetHeightRef(newFloorX, newFloorY);
			float newHeightSW = GetHeightRef(newFloorX, newFloorY + 1);
			float newHeightSE = GetHeightRef(newFloorX + 1, newFloorY + 1);

			//Interpolate height of droplet (We will use the four corners of the square instead of the triangles)
			float newHeight = (newHeightNW * (1 - newDX) + newHeightNE * newDX) * (1 - newDY) + (newHeightSW * (1 - newDX) + newHeightSE * newDX) * newDY;
			float dh = newHeight - height;

			// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
			float sedimentCapacity = glm::max(-dh * speed * water * sedimentCapacityFactor, minSedimentCapacity);

			//// If carrying more sediment than capacity, or if flowing uphill:
			if (sediment > sedimentCapacity || dh > 0) {
				// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
				float amountToDeposit = (dh > 0.f) ? glm::min(dh, sediment) : (sediment - sedimentCapacity) * depositSpeed;
				sediment -= amountToDeposit;

				// Add the sediment to the four nodes of the current cell using bilinear interpolation
				GetHeightRef(floorX, floorY) += amountToDeposit * (1 - dx) * (1 - dy);
				assert(GetHeightRef(floorX, floorY) == GetHeightRef(floorX, floorY));
				GetHeightRef(floorX + 1, floorY) += amountToDeposit * dx * (1 - dy);
				assert(GetHeightRef(floorX + 1, floorY) == GetHeightRef(floorX + 1, floorY));
				GetHeightRef(floorX, floorY + 1) += amountToDeposit * (1 - dx) * dy;
				assert(GetHeightRef(floorX, floorY + 1) == GetHeightRef(floorX, floorY + 1));
				GetHeightRef(floorX + 1, floorY + 1) += amountToDeposit * dx * dy;
				assert(GetHeightRef(floorX + 1, floorY + 1) == GetHeightRef(floorX + 1, floorY + 1));
			}
			else {
				// Erode a fraction of the droplet's current carry capacity.
				// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
				float amountToErode = glm::min(-dh, (sedimentCapacity - sediment) * erodeSpeed);
				if (amountToErode == 0.f)
					continue;

				////Get boundaries
				int left = glm::max(static_cast <int> (glm::ceil(pos.x - erosionRadius)), 0);
				int right = glm::min(static_cast <int> (glm::floor(pos.x + erosionRadius)), static_cast <int> (mWidth * (Chunk::CHUNK_SIZE - 1)));
				int top = glm::max(static_cast <int> (glm::ceil(pos.y - erosionRadius)), 0);
				int bottom = glm::min(static_cast <int> (glm::floor(pos.y + erosionRadius)), static_cast <int> (mHeight * (Chunk::CHUNK_SIZE - 1)));

				float** nodes = new float* [4 * erosionRadius * erosionRadius];
				float* weight = new float  [4 * erosionRadius * erosionRadius];
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
							nodes[count] = &GetHeightRef(x, y);
							weight[count] = weightedAmountToErode;
							count++;
							weightSum += weightedAmountToErode;
						}
					}
				}

				////Iterate over nodes within list and erode them
				for (int index = 0; index < count; index++)
				{
					float weighedErodeAmount = amountToErode * weight[index] / weightSum;
					float heighthere = *nodes[index];
					float deltaSediment = (*nodes[index] < weighedErodeAmount) ? *nodes[index] : weighedErodeAmount;
					*nodes[index] -= deltaSediment;
					assert(*nodes[index] == *nodes[index]);
					sediment += deltaSediment;
				}

				delete [] nodes;
				delete [] weight;
			}

			// Update droplet's speed and water content
			if (speed * speed + dh * gravity < 0)
				break;
			speed = glm::sqrt(speed * speed + dh * gravity);
			water *= (1 - evaporateSpeed);
		}
		if (i % 100000 == 0)
			std::cout << i << std::endl;
	}
}

float Terrain::GetHeight(float xCoord, float yCoord) 
{
	//Get coordinates
	int floorX = glm::floor(xCoord);
	int floorY = glm::floor(yCoord);
	float dx = xCoord - floorX;
	float dy = yCoord - floorX;

	//We are in south-eastern triangle
	if (dx < dy)
	{
		float heightNW = GetHeightRef(floorX, floorY);
		float heightSW = GetHeightRef(floorX, floorY + 1);
		float heightSE = GetHeightRef(floorX + 1, floorY + 1);

		float height = dy * (heightSW - heightNW) + heightNW;
		return dx * (heightSE - height) + height;
	}
	//We are in north-western triangle
	else
	{
		float heightNW = GetHeightRef(floorX, floorY);
		float heightNE = GetHeightRef(floorX + 1, floorY);
		float heightSE = GetHeightRef(floorX + 1, floorY + 1);

		float height = dx * (heightNE - heightNW) + heightNW;
		return dy * (heightSE - height) + height;
	}
}

void Terrain::Draw()
{
	//Set material coefficients
	GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
	glUniform4f(MaterialID, 0.5f, 0.4f, 0.1f, 50.f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < mWidth; i++)
	{
		for (int j = 0; j < mHeight; j++)
		{
			mChunkMap[i][j]->Draw();
		}
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}