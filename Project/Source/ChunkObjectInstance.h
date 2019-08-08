#pragma once

#include "ChunkObject.h"

class ChunkObjectInstance
{
public:
	ChunkObjectInstance(ChunkObject* c);

	void SetPosition(glm::vec3 position);
	void SetScaling(glm::vec3 scaling);
	void SetRotation(glm::vec3 axis, float angleDegrees);

	virtual glm::mat4 GetWorldMatrix() const;

private:
	ChunkObject* model;

	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::vec3 mRotationAxis;
	float rotationAngleInDegrees;
};