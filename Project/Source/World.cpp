//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
// Updated by Emanuel Sharma on 14/08/2019
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include <cstdio>

#include "World.h"
#include "Renderer.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"
#include "ParsingHelper.h"
#include "EventManager.h"
#include "TextureLoader.h"
#include "LightSource.h"
#include <GLFW/glfw3.h>

#include "World/Terrain/Terrain.h"
#include "World/Terrain/TerrainGenerator.h"
#include "PerlinNoise.h"
#include "ChunkPopulator.h"
#include "ObjectModel.h"
#include "ChunkObject.h"
#include "Skybox.h"
#include "World/Collisions/BoundingVolume.h"
#include "BSplineCamera.h"

using namespace std;
using namespace glm;

World* World::instance = nullptr;


World::World(char * scene)
{
	if (instance != nullptr)
	{
		delete instance;
	}
	instance = this;

	// SKYBOX
	mTotalTime = 0.0f;
	mSkybox = new Skybox();
	// TERRAIN
	// SET WORLD SEED HERE
	mpPerlin = new PerlinNoise(20);
	mpTerrainGenerator = new pg::terrain::TerrainGenerator(*mpPerlin);
	mpTerrain = new pg::terrain::Terrain(*mpTerrainGenerator);
	// WATER
	mpFBOs = new pg::water::WaterFrameBuffers();
	mpWaterRenderer = new pg::water::WaterRenderer(SHADER_WATER, *mpFBOs);
	// CHUNK OBJECTS
	pg::terrain::ChunkPopulator* chunkPopulator = new pg::terrain::ChunkPopulator(*mpTerrain, *mpPerlin);
	mpTerrain->AttachChunkPopulator(chunkPopulator);

	LoadScene(scene);
	mpTerrain->Start();

	// CAMERAS
	// First-person
	mCameraList.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCurrentCamera = 0;
	// Spline
	glm::vec2 splineCamOrigin = vec2(2.0f, 20.0f);
	glm::vec2 splineCamGenerationSize = vec2(500.0f, 500.0f);
	glm::vec2 splineCamHeightLimits = vec2(40, 100);
	unsigned int splineWaypointCount = 10;
	float splineSpeed = 50;
	mCameraList.push_back(new BSplineCamera(new BSpline(splineCamOrigin, splineCamGenerationSize, splineCamHeightLimits, splineWaypointCount), splineSpeed));
}

World::~World()
{
	// DELETE CAMERAS
	for (vector<Camera*>::iterator it = mCameraList.begin(); it < mCameraList.end(); ++it)
	{
		delete *it;
	}
	mCameraList.clear();

	delete mSkybox;
	delete mpPerlin;
	delete mpTerrainGenerator;
	delete mpTerrain;
	delete mpFBOs;
	delete mpWaterRenderer;
}

World* World::GetInstance()
{
    return instance;
}

bool World::CheckCollisions(float x, float y, BoundingVolume* volume) 
{
    return World::GetInstance()->GetTerrain()->CheckCollisionsAt(x, y, volume);
}

void World::Update(float dt)
{
	// SKYBOX
	mTotalTime += dt;
	if (mTotalTime < 0) { //overflow protection
		mTotalTime = 0 + dt;
	}
	mDayPhase = fmod(mTotalTime, 100.0f) > 50.0f;
	mDayRatio = fmod(mTotalTime, 50.0f) / 50.0f;
	if (mDayPhase) {
		(mLightList[0])->setColor(mDayRatio);
	}
	else {
		(mLightList[0])->setColor(1.0f - mDayRatio);
	}

	// User Inputs
	// 1 2 to change the Camera
    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
    {
        mCurrentCamera = 0;
    }
    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
    {
        mCurrentCamera = 1;
    }
	// Update current Camera
	mCameraList[mCurrentCamera]->Update(dt);
}

void World::Draw()
{
	Renderer::BeginFrame();

	// SKYBOX
	// Set shader
	Renderer::SetShader(SHADER_SKYBOX);
	glUseProgram(Renderer::GetShaderProgramID());
	// View-Projection uniforms
	GLuint VMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	GLuint PMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
	mat4 V = mat4(mat3(mCameraList[mCurrentCamera]->GetViewMatrix())); //removing translate component for skybox
	mat4 P = mCameraList[mCurrentCamera]->GetProjectionMatrix();
	glUniformMatrix4fv(VMatrixLocation, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(PMatrixLocation, 1, GL_FALSE, &P[0][0]);
	// Draw skybox
	mSkybox->Draw(mDayPhase, mDayRatio);

	// TERRAIN
	// Set shader
	Renderer::SetShader(SHADER_SOLID_COLOR);
	glUseProgram(Renderer::GetShaderProgramID());
	// View-Projection uniforms
	VMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	PMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
	V = mCameraList[mCurrentCamera]->GetViewMatrix();
	P = mCameraList[mCurrentCamera]->GetProjectionMatrix();
	glUniformMatrix4fv(VMatrixLocation, 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(PMatrixLocation, 1, GL_FALSE, &P[0][0]);
	// Set light uniforms
	SetLights();
	// Set material coefficients
	GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glm::mat4 worldMatrix(1.0f); // Coordinates of terrain are already in world space
	glUniform4f(MaterialID, 0.5f, 0.45f, 0.05f, 50.f);
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	// Draw to FBOs
	// This code was for rendering to the refraction and frame buffers but does not work
	//mpFBOs->BindReflectionFrameBuffer();
	//mpTerrain->Draw();
	//mpFBOs->BindRefractionFrameBuffer();
	//mpTerrain->Draw();
	//mpFBOs->UnbindCurrentFrameBuffer();
	// Draw terrain
	mpTerrain->Draw();
	// Draw water
	mpTerrain->DrawWater(*mpWaterRenderer);

    Renderer::CheckForErrors();
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
			if (result == "light")
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

const Camera* World::GetCurrentCamera() const
{
    return mCameraList[mCurrentCamera];
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