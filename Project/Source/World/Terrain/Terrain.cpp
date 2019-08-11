#include "Terrain.h"
#include "TerrainChunk.h"
#include "TerrainGenerator.h"

#include <glm/glm.hpp>

#if defined(GLM_PLATFORM_APPLE) || defined(GLM_PLATFORM_LINUX)
#include "../../World.h"
#include "../../Camera.h"
#include "../../Renderer.h"
#include "../../ChunkPopulator.h"
#else
#include "..\..\World.h"
#include "..\..\Camera.h"
#include "..\..\Renderer.h"
#include "..\..\ChunkPopulator.h"
#endif

namespace pg
{
	namespace terrain
	{

		Terrain::Terrain(const TerrainGenerator& terrainGenerator)
			: mTerrainGenerator(terrainGenerator)
			, initialGenDone(false)
		{ }



		Terrain::Terrain(const Terrain& orig)
			: chunkPopulator(orig.chunkPopulator)
			, mTerrainGenerator(orig.mTerrainGenerator)
			, mAesthetic(orig.mAesthetic)
			, mChunkMap(orig.mChunkMap)
			, initialGenDone(false)
		{ }


		Terrain::~Terrain()
		{
			// We will leave terrainGenerator undeleted in case something else is using it
			for (auto it = mChunkMap.begin(); it != mChunkMap.end(); it++)
			{
				delete it->second;
			}
		}

		void Terrain::Start()
		{
			int range = 2;
			int numChunks = (2 * range) * (2 * range);
			int i = 0;
			// Pre-generate a perimiter of chunks around origin

			for (int x = -range; x < range; x++)
			{
				for (int y = -range; y < range; y++)
				{
					// Get chunk at coordinates, generate new one if it does not yet exist
					TerrainChunk& crrtChunk = GetChunkAt(x, y);

					std::cout << i++ << "/" << numChunks << std::endl;
				}
			}
			initialGenDone = true;
		}

		void Terrain::Draw()
		{
			// Get view information
			const Camera& crrtCamera = *World::GetInstance()->GetCurrentCamera();
			const glm::vec3 pos = crrtCamera.GetPosition();
			const float viewDist = Camera::DIST_FAR_PLANE;
			const glm::vec3 lookAt = crrtCamera.GetLookAt();
			// Get range of chunks within draw distance
			const int chunkLoadRadius = static_cast <int> (viewDist / TerrainChunk::CHUNK_SIZE) + 1;
			const int centerChunkX = static_cast <int> (glm::floor(pos.x / TerrainChunk::CHUNK_SIZE));
			const int centerChunkY = static_cast <int> (glm::floor(pos.z / TerrainChunk::CHUNK_SIZE));
			const float LODFactor = static_cast <float> (TerrainChunk::MIN_LOD - 1) / static_cast <float> (chunkLoadRadius);

			int drawnChunks = 0;

			// Loop over chunks within draw distance
			for (int offsetX = -chunkLoadRadius; offsetX < chunkLoadRadius; offsetX++)
			{
				unsigned short LODX = (offsetX < 0) ? -static_cast <short> (LODFactor * offsetX) : static_cast <short> (LODFactor * offsetX);
				for (int offsetY = -chunkLoadRadius; offsetY < chunkLoadRadius; offsetY++)
				{
					//Check if chunk is within view (we will cull chunks behind camera)
					glm::vec3 topLeft = glm::vec3((centerChunkX + offsetX) * TerrainChunk::CHUNK_SIZE, 0.f, (centerChunkY + offsetY) * TerrainChunk::CHUNK_SIZE) - pos;
					glm::vec3 topRight = topLeft + glm::vec3(TerrainChunk::CHUNK_SIZE, 0.f, 0.f);
					glm::vec3 bottomLeft = topLeft + glm::vec3(0.f, 0.f, TerrainChunk::CHUNK_SIZE);
					glm::vec3 bottomRight = topLeft + glm::vec3(TerrainChunk::CHUNK_SIZE, 0.f, TerrainChunk::CHUNK_SIZE);
					// If any of chunk corners are in front of near plane, do not cull
					if (glm::dot(lookAt, topLeft) > 0.f && glm::dot(lookAt, topRight) > 0.f && glm::dot(lookAt, bottomLeft) > 0.f && glm::dot(lookAt, bottomRight) > 0.f)
					{
						continue;
					}

					// Get chunk at coordinates, generate new one if it does not yet exist
					TerrainChunk& crrtChunk = GetChunkAt(centerChunkX + offsetX, centerChunkY + offsetY);

					// Get LOD of chunk
					unsigned short LODY = (offsetY < 0) ? -static_cast <short> (LODFactor * offsetY) : static_cast <short> (LODFactor * offsetY);
					unsigned int LOD = glm::max(LODX, LODY);
					LOD = (LOD == 0) ? 0 : LOD - 1;

					// Draw Chunk
					crrtChunk.Draw(LOD);
					drawnChunks++;
				}
			}

			std::cout << "Chunks rendered " << drawnChunks << "\n";
		}

		void Terrain::DrawWater(water::WaterRenderer& waterRenderer)
		{
			// Get view information
			const Camera& crrtCamera = *World::GetInstance()->GetCurrentCamera();
			const glm::vec3 pos = crrtCamera.GetPosition();
			const float viewDist = Camera::DIST_FAR_PLANE;
			const glm::vec3 lookAt = crrtCamera.GetLookAt();
			// Get range of chunks within draw distance
			const int chunkLoadRadius = static_cast <int> (viewDist / TerrainChunk::CHUNK_SIZE) + 1;
			const int centerChunkX = static_cast <int> (glm::floor(pos.x / TerrainChunk::CHUNK_SIZE));
			const int centerChunkY = static_cast <int> (glm::floor(pos.z / TerrainChunk::CHUNK_SIZE));
			waterRenderer.Start();
			// Loop over chunks within draw distance
			for (int offsetX = -chunkLoadRadius; offsetX < chunkLoadRadius; offsetX++)
			{
				for (int offsetY = -chunkLoadRadius; offsetY < chunkLoadRadius; offsetY++)
				{
					//Check if chunk is within view (we will cull chunks behind camera)
					glm::vec3 topLeft = glm::vec3((centerChunkX + offsetX) * TerrainChunk::CHUNK_SIZE, 0.f, (centerChunkY + offsetY) * TerrainChunk::CHUNK_SIZE) - pos;
					glm::vec3 topRight = topLeft + glm::vec3(TerrainChunk::CHUNK_SIZE, 0.f, 0.f);
					glm::vec3 bottomLeft = topLeft + glm::vec3(0.f, 0.f, TerrainChunk::CHUNK_SIZE);
					glm::vec3 bottomRight = topLeft + glm::vec3(TerrainChunk::CHUNK_SIZE, 0.f, TerrainChunk::CHUNK_SIZE);
					// If any of chunk corners are in front of near plane, do not cull
					if (glm::dot(lookAt, topLeft) > 0.f && glm::dot(lookAt, topRight) > 0.f && glm::dot(lookAt, bottomLeft) > 0.f && glm::dot(lookAt, bottomRight) > 0.f)
					{
						continue;
					}

					// Get chunk at coordinates, generate new one if it does not yet exist
					TerrainChunk& crrtChunk = GetChunkAt(centerChunkX + offsetX, centerChunkY + offsetY);
					waterRenderer.Draw(crrtChunk.mWater);
				}
			}
			waterRenderer.Stop();
		}

		float Terrain::GetHeightAt(const int xCoord, const int yCoord) const
		{
			const int chunkX = glm::floor(static_cast<float> (xCoord) / TerrainChunk::CHUNK_SIZE);
			const int chunkY = glm::floor(static_cast<float> (yCoord) / TerrainChunk::CHUNK_SIZE);
			const int dx = (xCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;
			const int dy = (yCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;

			auto it = mChunkMap.find({ chunkX, chunkY });
			// DO not call GetHeightAt for unloaded parts of terrain!
			assert(it != mChunkMap.end());
			TerrainChunk* chunk = it->second;

			return chunk->mHeightMap[dx][dy];
		}

		float* Terrain::GetHeightRefAt(const int xCoord, const int yCoord)
		{
			const int chunkX = glm::floor(static_cast<float> (xCoord) / TerrainChunk::CHUNK_SIZE);
			const int chunkY = glm::floor(static_cast<float> (yCoord) / TerrainChunk::CHUNK_SIZE);
			const int dx = (xCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;
			const int dy = (yCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;

			auto it = mChunkMap.find({ chunkX, chunkY });
			// DO not call GetHeightAt for unloaded parts of terrain!
			assert(it != mChunkMap.end());
			TerrainChunk* chunk = it->second;

			return &(chunk->mHeightMap[dx][dy]);
		}

		void Terrain::SetHeightAt(const int xCoord, const int yCoord, const float value)
		{
			const int chunkX = glm::floor(static_cast<float> (xCoord) / TerrainChunk::CHUNK_SIZE);
			const int chunkY = glm::floor(static_cast<float> (yCoord) / TerrainChunk::CHUNK_SIZE);
			const int dx = (xCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;
			const int dy = (yCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;

			auto it = mChunkMap.find({ chunkX, chunkY });
			// DO not call GetHeightAt for unloaded parts of terrain!
			assert(it != mChunkMap.end());
			TerrainChunk* chunk = it->second;

			chunk->mHeightMap[dx][dy] = value;
		}

		glm::vec3 Terrain::GetNormalAt(const int xCoord, const int yCoord) const
		{
			const int chunkX = glm::floor(static_cast<float> (xCoord) / TerrainChunk::CHUNK_SIZE);
			const int chunkY = glm::floor(static_cast<float> (yCoord) / TerrainChunk::CHUNK_SIZE);
			const int dx = (xCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;
			const int dy = (yCoord % TerrainChunk::CHUNK_SIZE + TerrainChunk::CHUNK_SIZE) % TerrainChunk::CHUNK_SIZE;

			auto it = mChunkMap.find({ chunkX, chunkY });
			// DO not call GetHeightAt for unloaded parts of terrain!
			assert(it != mChunkMap.end());
			TerrainChunk* chunk = it->second;

			return chunk->mNormalMap[dx][dy];
		}

		float Terrain::GetHeightAt(const float xCoord, const float yCoord) const
		{
			//Get coordinates
			int floorX = glm::floor(xCoord);
			int floorY = glm::floor(yCoord);
			float dx = xCoord - floorX;
			float dy = yCoord - floorY;

            //We are in south-eastern triangle
            if (dx < dy)
            {
                float heightNW = GetHeightAt(floorX, floorY);
                float heightSW = GetHeightAt(floorX, floorY + 1);
                float heightSE = GetHeightAt(floorX + 1, floorY + 1);

                float height = dy * (heightSW - heightNW) + heightNW;
                return dx * (heightSE - height) + height;
            }
                //We are in north-western triangle
            else
            {
                float heightNW = GetHeightAt(floorX, floorY);
                float heightNE = GetHeightAt(floorX + 1, floorY);
                float heightSE = GetHeightAt(floorX + 1, floorY + 1);

                float height = dx * (heightNE - heightNW) + heightNW;
                return dy * (heightSE - height) + height;
            }
		}

		glm::vec3 Terrain::GetNormalAt(const float xCoord, const float yCoord) const
		{
			//Get coordinates
			int floorX = glm::floor(xCoord);
			int floorY = glm::floor(yCoord);
			float dx = xCoord - floorX;
			float dy = yCoord - floorX;

			//We are in south-eastern triangle
			if (dx < dy)
			{
				glm::vec3 normalNW = GetNormalAt(floorX, floorY);
				glm::vec3 normalSW = GetNormalAt(floorX, floorY + 1);
				glm::vec3 normalSE = GetNormalAt(floorX + 1, floorY + 1);

				glm::vec3 normal = dy * (normalSW - normalNW) + normalNW;
				return dx * (normalSE - normal) + normal;
			}
			//We are in north-western triangle
			else
			{
				glm::vec3 normalNW = GetNormalAt(floorX, floorY);
				glm::vec3 normalNE = GetNormalAt(floorX + 1, floorY);
				glm::vec3 normalSE = GetNormalAt(floorX + 1, floorY + 1);

				glm::vec3 normal = dx * (normalNE - normalNW) + normalNW;
				return dy * (normalSE - normal) + normal;
			}
		}

		float Terrain::GetHumidityAt(const int mXCoord, const int mYCoord) const
		{
			return mTerrainGenerator.GetHumidityAt(mXCoord, mYCoord);
		}

		float Terrain::GetTemperatureAt(const int mXCoord, const int mYCoord) const
		{
			return mTerrainGenerator.GetTemperatureAt(mXCoord, mYCoord);
		}

		TerrainChunk& Terrain::GetChunkAt(const int xCoord, const int yCoord)
		{
			//Try to find chunk
			auto it = mChunkMap.find({ xCoord, yCoord });
			//Generate new chunk if it does not exist
			if (it == mChunkMap.end())
			{

                if (!initialGenDone || GenerateInfiniteTerrain) 
                {
				    //Create chunk
				    TerrainChunk* chunk = new TerrainChunk(*this, TerrainChunk::CHUNK_SIZE * xCoord, TerrainChunk::CHUNK_SIZE * yCoord);
				    // Add neighboring chunks
				    // NORTH
				    it = mChunkMap.find({ xCoord, yCoord - 1 });
				    if (it != mChunkMap.end())
				    {
				    	chunk->SetNorthChunk(it->second);
				    }
				    // WEST
				    it = mChunkMap.find({ xCoord - 1, yCoord });
				    if (it != mChunkMap.end())
				    {
				    	chunk->SetWestChunk(it->second);
				    }
				    // SOUTH
				    it = mChunkMap.find({ xCoord, yCoord + 1 });
				    if (it != mChunkMap.end())
				    {
				    	chunk->SetSouthChunk(it->second);
				    }
				    // EAST
				    it = mChunkMap.find({ xCoord + 1, yCoord });
				    if (it != mChunkMap.end())
				    {
				    	chunk->SetEastChunk(it->second);
				    }
				    mTerrainGenerator.FillChunk(*chunk);
				    mChunkMap.insert({ { xCoord, yCoord }, chunk });

				    chunkPopulator->PopulateChunk(chunk);

				    return *chunk;
                }
                else {
                    //todo fix this hack
					auto i = mChunkMap.find({ 0,0 });
					return *i->second;
				}
			}
			else
			{
				return *it->second;
			}
		}

		void Terrain::AttachChunkPopulator(ChunkPopulator * c)
		{
			chunkPopulator = c;
		}

		void Terrain::AddChunkObject(ChunkObject * o)
		{
			chunkPopulator->AddObject(o);
		}

	}
}