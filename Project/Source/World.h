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

class Terrain;
class Camera;
class Model;
class Animation;
class AnimationKey;
class ParticleSystem;
class ParticleDescriptor;
class LightSource;

class World
{
public:
	World();
	~World();
	
    static World* GetInstance();

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);
    Animation* FindAnimation(ci_string animName);
    AnimationKey* FindAnimationKey(ci_string keyName);
    ParticleDescriptor* FindParticleDescriptor(ci_string name);

    const Camera* GetCurrentCamera() const;
	void SetLights();
	void AddLightSource(LightSource* b);
	void RemoveLightSource(LightSource* b);
	void AddBillboard(Billboard* b);
	void RemoveBillboard(Billboard* b);
    void AddParticleSystem(ParticleSystem* particleSystem);
    void RemoveParticleSystem(ParticleSystem* particleSystem);
    void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);

    
private:
    static World* instance;
    
	Terrain* mpTerrain;
	std::vector<Model*> mModel;
    std::vector<Animation*> mAnimation;
    std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
    std::vector<ParticleSystem*> mParticleSystemList;
	std::vector<ParticleDescriptor*> mParticleDescriptorList;
	std::vector<LightSource*> mLightList;
	unsigned int mCurrentCamera;

    BillboardList* mpBillboardList;
};
