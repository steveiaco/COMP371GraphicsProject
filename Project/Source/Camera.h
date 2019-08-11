//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos);
	virtual ~Camera();

	virtual void Update(float dt) = 0;

	glm::vec3 GetPosition() const { return mPosition; }
	glm::vec3 GetLookAt() const;

	virtual glm::mat4 GetViewMatrix() const = 0;
	virtual glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix() const;

	static const float FOV_Y;
	static const float ASPECT_RATIO;
	static const float DIST_NEAR_PLANE;
	static const float DIST_FAR_PLANE;

protected:
	glm::vec3 mPosition;
};
