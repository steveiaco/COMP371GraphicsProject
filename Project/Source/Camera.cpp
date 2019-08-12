//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Camera.h"
#include <glm/gtx/transform.hpp>

using namespace glm;

const float Camera::FOV_Y = 45.f;
const float Camera::ASPECT_RATIO = 4.0f / 3.f;
const float Camera::DIST_NEAR_PLANE = 0.1f;
const float Camera::DIST_FAR_PLANE = 700.0f;

Camera::Camera()
{
}

Camera::Camera(glm::vec3 pos)
	: mPosition(pos)
{
}

Camera::~Camera()
{
}

mat4 Camera::GetViewProjectionMatrix() const
{
	mat4 viewProjection = GetProjectionMatrix() * GetViewMatrix();

	return viewProjection;
}

vec3 Camera::GetLookAt() const
{
	mat4 V = GetViewMatrix();

	return vec3(V[0][2], V[1][2], V[2][2]);
}

mat4 Camera::GetProjectionMatrix() const
{
	return perspective(FOV_Y, ASPECT_RATIO, DIST_NEAR_PLANE, DIST_FAR_PLANE);
}
