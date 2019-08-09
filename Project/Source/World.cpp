//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include <cstdio>

#include "World.h"
#include "Renderer.h"
#include "ParsingHelper.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"

#include "CubeModel.h"
#include "SphereModel.h"
#include "ObjectModel.h"
#include "ChunkObject.h"
#include "Animation.h"
#include "Billboard.h"
#include <GLFW/glfw3.h>
#include "EventManager.h"
#include "TextureLoader.h"
#include "LightSource.h"

#include "World/Terrain/Terrain.h"
#include "World/Terrain/TerrainGenerator.h"
#include "PerlinNoise.h"
#include "ChunkPopulator.h"

#include "ParticleDescriptor.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

using namespace std;
using namespace glm;

World* World::instance;


World::World(char * scene)
{
    instance = this;


	mpPerlin = new PerlinNoise();
	mpTerrainGenerator = new pg::terrain::TerrainGenerator(*mpPerlin);
	mpTerrain = new pg::terrain::Terrain(*mpTerrainGenerator);

	pg::terrain::ChunkPopulator* chunkPopulator = new pg::terrain::ChunkPopulator(*mpTerrain, *mpPerlin);
	mpTerrain->AttachChunkPopulator(chunkPopulator);

	LoadScene(scene);

	mpTerrain->Start();
	// Setup Camera
	mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCurrentCamera = 0;
    
#if defined(PLATFORM_OSX)
    int billboardTextureID = TextureLoader::LoadTexture("Textures/Particle.png");
#else
    int billboardTextureID = TextureLoader::LoadTexture("../Assets/Textures/Particle.png");
#endif
    assert(billboardTextureID != 0);
    
    mpBillboardList = new BillboardList(2048, billboardTextureID);
}

World::~World()
{
	// Models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		delete *it;
	}

	mModel.clear();

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		delete *it;
	}

	mAnimation.clear();

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		delete *it;
	}

	mAnimationKey.clear();

	// Camera
	for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	{
		delete *it;
	}
	mCamera.clear();
    
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it < mParticleSystemList.end(); ++it)
    {
        delete *it;
    }
    mParticleSystemList.clear();
    
    for (vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
    {
        delete *it;
    }
    mParticleDescriptorList.clear();

	delete mpTerrain;
    
	delete mpBillboardList;
}

World* World::GetInstance()
{
    return instance;
}

void World::Update(float dt)
{
	// User Inputs
	// 0 1 2 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}

    // Update animation and keys
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        (*it)->Update(dt);
    }
	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}
    
    // Update billboards
    
    for (vector<ParticleSystem*>::iterator it = mParticleSystemList.begin(); it != mParticleSystemList.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    mpBillboardList->Update(dt);

}

void World::Draw()
{
	Renderer::BeginFrame();
	
	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());

	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	GLuint PMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");

	// Send the view projection constants to the shader
	mat4 V = mCamera[mCurrentCamera]->GetViewMatrix();
	mat4 P = mCamera[mCurrentCamera]->GetProjectionMatrix();
	glUniformMatrix4fv(VMatrixLocation, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(PMatrixLocation, 1, GL_FALSE, &P[0][0]);

	SetLights();

	//Set material coefficients
	GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
	glUniform4f(MaterialID, 0.5f, 0.4f, 0.1f, 50.f);
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 worldMatrix(1.0f);
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	mpTerrain->Draw();

	// Draw models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Draw();
	}

	// Draw Path Lines
	
	// Set Shader for path lines
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());

	// Send the view projection constants to the shader
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

	for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
	{
		mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
		glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

		(*it)->Draw();
	}

    Renderer::CheckForErrors();
    
    // Draw Billboards
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mpBillboardList->Draw();
    glDisable(GL_BLEND);


	// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);

	Renderer::EndFrame();
}

void World::LoadScene(const char * scene_path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if(input.fail() )
	{	 
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while( std::getline( input, item, '[' ) )   
	{
        ci_istringstream iss( item );

		ci_string result;
		if( std::getline( iss, result, ']') )
		{
			if( result == "cube" )
			{
				// Box attributes
				CubeModel* cube = new CubeModel();
				cube->Load(iss);
				mModel.push_back(cube);
			}
            else if( result == "sphere" )
            {
                SphereModel* sphere = new SphereModel();
                sphere->Load(iss);
                mModel.push_back(sphere);
            }
			else if ( result == "animationkey" )
			{
				AnimationKey* key = new AnimationKey();
				key->Load(iss);
				mAnimationKey.push_back(key);
			}
			else if (result == "animation")
			{
				Animation* anim = new Animation();
				anim->Load(iss);
				mAnimation.push_back(anim);
			}
			else if (result == "particledescriptor")
			{
				ParticleDescriptor* psd = new ParticleDescriptor();
				psd->Load(iss);
				AddParticleDescriptor(psd);
			}
			else if (result == "light")
			{
				// We want no more than 8 lights at a time
				if (mLightList.size() == 8)
				{
					fprintf(stderr, "You can have no more than 8 lights at a time! Please remove some lights.");
					exit(-1);
				}
				LightSource* lightSource = new LightSource();
				lightSource->Load(iss);
				AddLightSource(lightSource);
			}
			else if (result == "model") 
			{
				ObjectModel* obj = new ObjectModel();
				obj->Load(iss);
				mModel.push_back(obj);
			}
			else if (result == "chunkobject") 
			{
				ChunkObject* obj = new ChunkObject();
				obj->Load(iss);
				mpTerrain->AddChunkObject(obj);
			}
			else if ( result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
	    }
	}
	input.close();

	// Set Animation vertex buffers
	for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		// Draw model
		(*it)->CreateVertexBuffer();
	}
}

void World::SetLights()
{
	//Send the light data to the shader
	GLuint LightCountLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "NumLights");
	glUniform1i(LightCountLocation, mLightList.size());
	for (int i = 0; i < mLightList.size(); i++)
	{
		char sUniformName[32];
        // sprintf_s is part of an optional annex to the C++11 specification
        // snprintf is safer and is part of the core standard and provides the same functionality
        snprintf(sUniformName, 32, "LightPositions[%i]", i);
		GLuint WorldLightPositionLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);
        snprintf(sUniformName, 32, "LightColors[%i]", i);
		GLuint LightColorLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);
        snprintf(sUniformName, 32, "LightAttenuations[%i]", i);
		GLuint LightAttenuationLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);

		glUniform4fv(WorldLightPositionLocation, 1, reinterpret_cast<GLfloat*>(&mLightList[i]->GetPosition()[0]));
		glUniform3fv(LightColorLocation, 1, reinterpret_cast<GLfloat*>(&mLightList[i]->GetColor()[0]));
		glUniform3fv(LightAttenuationLocation, 1, reinterpret_cast<GLfloat*>(&mLightList[i]->GetAttenuation()[0]));
	}
}

Animation* World::FindAnimation(ci_string animName)
{
	for (std::vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
	{
		if ((*it)->GetName() == animName)
		{
			return *it;
		}
	}
	return nullptr;
}

AnimationKey* World::FindAnimationKey(ci_string keyName)
{
    for(std::vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        if((*it)->GetName() == keyName)
        {
            return *it;
        }
    }
    return nullptr;
}

const Camera* World::GetCurrentCamera() const
{
     return mCamera[mCurrentCamera];
}

void World::AddLightSource(LightSource* ls)
{
	mLightList.push_back(ls);
}

void World::RemoveLightSource(LightSource* ls)
{
	vector<LightSource*>::iterator it = std::find(mLightList.begin(), mLightList.end(), ls);
	mLightList.erase(it);
}

void World::AddBillboard(Billboard* b)
{
	mpBillboardList->AddBillboard(b);
}

void World::RemoveBillboard(Billboard* b)
{
	mpBillboardList->RemoveBillboard(b);
}

void World::AddParticleSystem(ParticleSystem* particleSystem)
{
    mParticleSystemList.push_back(particleSystem);
}

void World::RemoveParticleSystem(ParticleSystem* particleSystem)
{
    vector<ParticleSystem*>::iterator it = std::find(mParticleSystemList.begin(), mParticleSystemList.end(), particleSystem);
    mParticleSystemList.erase(it);
}

void World::AddParticleDescriptor(ParticleDescriptor* particleDescriptor)
{
    mParticleDescriptorList.push_back(particleDescriptor);
}

ParticleDescriptor* World::FindParticleDescriptor(ci_string name)
{
    for(std::vector<ParticleDescriptor*>::iterator it = mParticleDescriptorList.begin(); it < mParticleDescriptorList.end(); ++it)
    {
        if((*it)->GetName() == name)
        {
            return *it;
        }
    }
    return nullptr;
}
