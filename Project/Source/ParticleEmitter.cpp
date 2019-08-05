//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//         with help from Jordan Rutty
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "ParticleEmitter.h"
#include "Model.h"

using namespace glm;

ParticleEmitter::ParticleEmitter(glm::vec3 position, const Model* parent)
: mpParent(parent), mPosition(position)
{
}


// This is a point emitter, nothing is random
// As a small extra task, you could create derived classes
// for more interesting emitters such as line emitters or circular emitters
// In these cases, you would sample a random point on these shapes
glm::vec3 ParticleEmitter::GetPosition()
{
	//There are two solutions here, one only translates the emitter, the other rotates and scales it with the parent obbject. For this assignment, the translating solution would suffice. However, other emitters placed more particularly might require a more robust solution.
	/*
	glm::vec3 parentWorldPos = glm::vec3(0,0,0);
	
	if (mpParent != nullptr)
	{
		//Instead of using GetPosition(), we will use GetWorldMatrix(). This way, we can keep track of the animated position as well.
		glm::mat4 parentWorldMatrix = mpParent->GetWorldMatrix();
		parentWorldPos = glm::vec3(parentWorldMatrix[3][0], parentWorldMatrix[3][1], parentWorldMatrix[3][2]);
	}

	glm::vec3 position = parentWorldPos + mPosition;
	*/

	//Alternatively, we could account for the rotation and scaling of the parent object as well. This might be important depending on how the particle emitter is defined.if (mpParent != nullptr)
	glm::mat4 parentWorldMatrix = glm::mat4(1.f);

	if (mpParent != nullptr)
	{
		parentWorldMatrix = mpParent->GetWorldMatrix();
	}

	glm::vec3 position = parentWorldMatrix * glm::vec4(mPosition, 1.f);

    return position;
}




