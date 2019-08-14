//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "ParsingHelper.h"
#include <vector>

class Camera;
class Model;
class ChunkObject;
class LightSource;
class Skybox;

namespace pg
{
	namespace terrain
	{
		class Terrain;
		class TerrainGenerator;
	}

	namespace water
	{
		class WaterFrameBuffer;
		class WaterRenderer;
		class WaterFrameBuffers;
	}
}
class PerlinNoise;
class BoundingVolume;

class World
{
public:
	World(char * scene);
	~World();
	
    static World* GetInstance();
    static bool CheckCollisions(float x, float y, BoundingVolume* volume);

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);

    const Camera* GetCurrentCamera() const;
    pg::terrain::Terrain* GetTerrain() { return mpTerrain; };

	void SetLights();
	void AddLightSource(LightSource* b);
	void RemoveLightSource(LightSource* b);

    
private:
    static World* instance;

	PerlinNoise* mpPerlin;
	pg::terrain::TerrainGenerator* mpTerrainGenerator;
	pg::terrain::Terrain* mpTerrain;
	pg::water::WaterFrameBuffers* mpFBOs;
	pg::water::WaterRenderer* mpWaterRenderer;
	Skybox* mSkybox;
	float mTotalTime;
	bool mDayPhase;
	float mDayRatio;

	std::vector<Camera*> mCameraList;
	std::vector<LightSource*> mLightList;
	unsigned int mCurrentCamera;
};
