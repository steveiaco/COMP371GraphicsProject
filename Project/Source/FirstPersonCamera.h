//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"

class BoundingVolume;

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera(glm::vec3 position);
	virtual ~FirstPersonCamera();

	virtual void Update(float dt);

	virtual glm::mat4 GetViewMatrix() const;

private:	
	float mHorizontalAngle; // horizontal angle
	float mVerticalAngle;   // vertical angle

	float mSpeed;			// World units per second
	float mAngularSpeed;    // Degrees per pixel
	float mVelocity;        // Used for jumping
	bool  mFreeMode;
	bool  mJumping;
	int   mOldSpaceBarState;
	float mPreviousHeight;
    int   mOldFreeModeKeyState;
    BoundingVolume* mBoundingVolume;

	glm::vec3 mLookAt;

    float computeHeight(float dt);
    void handleInput(float dt);
};
