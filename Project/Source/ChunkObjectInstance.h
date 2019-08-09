#pragma once

#include "ChunkObject.h"
#include <glm/glm.hpp>


class ChunkObjectInstance
{
public:
	ChunkObjectInstance(ChunkObject* c);

	void SetPosition(glm::vec3 position);
	void SetScaling(glm::vec3 scaling);
	void SetRotation(glm::vec3 angle);

	ChunkObject* GetModel() const { return model; }

	virtual glm::mat4 GetWorldMatrix() const;
	virtual void Draw();


private:
	ChunkObject* model;

	//Position
	glm::vec3 mPosition;

	//Scaling
	glm::vec3 mScaling;

	//Rotation angles in euler angles
	glm::vec3 mRotation;

	static int currentVAO;
	static int currentVBO;
};