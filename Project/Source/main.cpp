//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Renderer.h"
#include "World.h"
#include "EventManager.h"
#include "TextureLoader.h"

int main(int argc, char*argv[])
{
	EventManager::Initialize();
	Renderer::Initialize();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	char * scene;
#if defined(PLATFORM_OSX)
	scene = "../Assets/Scenes/JustTrees.scene";
#else
	scene = "../Assets/Scenes/JustTrees.scene";
#endif

	World world(scene);

	// Main Loop
	do
	{
		// Update Event Manager - Frame time / input / events processing 
		EventManager::Update();

		// Update World
		float dt = EventManager::GetFrameTime();
		world.Update(dt);

		// Draw World
		world.Draw();        
	}
	while(EventManager::ExitRequested() == false);

	Renderer::Shutdown();
	EventManager::Shutdown();

	return 0;
}
