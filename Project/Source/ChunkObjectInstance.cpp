#include "ChunkObjectInstance.h"

ChunkObjectInstance::ChunkObjectInstance(ChunkObject * c) : model(c) 
{ 
	mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	mScaling = glm::vec3(0.0f, 0.0f, 0.0f);
	mRotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	rotationAngleInDegrees = 0.0f;
}

void ChunkObjectInstance::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void ChunkObjectInstance::SetScaling(glm::vec3 scaling)
{
	mScaling = scaling;
}

void ChunkObjectInstance::SetRotation(glm::vec3 axis, float angleDegrees)
{
	mRotationAxis = axis;
	rotationAngleInDegrees = angleDegrees;
}
