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
#include "Billboard.h"
#include <vector>

#include "World/Water/WaterFrameBuffers.h"
#include "World/Water/WaterRenderer.h"
#include "SphereModel.h"

class Camera;
class Model;
class ChunkObject;
class Animation;
class AnimationKey;
class ParticleSystem;
class ParticleDescriptor;
class LightSource;
class Skybox;

namespace pg
{
	namespace terrain
	{
		class Terrain;
		class TerrainGenerator;
	}
}
class PerlinNoise;

class World
{
public:
	World(char * scene);
	~World();
	
    static World* GetInstance();

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);
    Animation* FindAnimation(ci_string animName);
    AnimationKey* FindAnimationKey(ci_string keyName);
    ParticleDescriptor* FindParticleDescriptor(ci_string name);

    const Camera* GetCurrentCamera() const;
    const pg::terrain::Terrain* GetTerrain() const { return mpTerrain; };

	void SetLights();
	void AddLightSource(LightSource* b);
	void RemoveLightSource(LightSource* b);
	void AddBillboard(Billboard* b);
	void RemoveBillboard(Billboard* b);
    void AddParticleSystem(ParticleSystem* particleSystem);
    void RemoveParticleSystem(ParticleSystem* particleSystem);
    void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);

	void AddSphere(glm::vec3 pos, float size)
	{
		SphereModel* pSphere = new SphereModel();
		pSphere->SetPosition(pos);
		pSphere->SetScaling(size * glm::vec3(1.f, 1.f, 1.f));
		mModel.push_back(pSphere);
	}

    
private:
    static World* instance;

	PerlinNoise* mpPerlin;
	pg::terrain::TerrainGenerator* mpTerrainGenerator;
	pg::terrain::Terrain* mpTerrain;
	Skybox* mSkybox;
	float mTotalTime;
	bool mDayPhase;
	float mDayRatio;
	pg::water::WaterFrameBuffers mFBOs;
	pg::water::WaterRenderer mWaterRenderer;

	std::vector<Model*> mModel;
    std::vector<Animation*> mAnimation;
    std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
    std::vector<ParticleSystem*> mParticleSystemList;
	std::vector<ParticleDescriptor*> mParticleDescriptorList;
	std::vector<LightSource*> mLightList;
	std::vector<ChunkObject*> mChunkObject;
	unsigned int mCurrentCamera;

    //BillboardList* mpBillboardList;
};
