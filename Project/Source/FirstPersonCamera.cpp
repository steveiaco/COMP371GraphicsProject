//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "FirstPersonCamera.h"
#include "EventManager.h"
#include "World.h"
#include "World/Terrain/Terrain.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "World/Collisions/BoundingSphere.h"

#include <GLFW/glfw3.h>
#include <algorithm>

const static float BASE_HEIGHT = 10.0;
const static float CAMERA_RESPONSIVENESS = 10;
const static float GRAVITY = -1000;
const static float JUMP_FORCE = 400;

using namespace glm;

FirstPersonCamera::FirstPersonCamera(glm::vec3 position) :  Camera(position), mLookAt(0.0f, 0.0f, -1.0f), mHorizontalAngle(90.0f), mVerticalAngle(0.0f), mSpeed(45.0f), mAngularSpeed(2.5f), mVelocity(0.0f), mFreeMode(false)
{
    mPreviousHeight = 0;
    mOldSpaceBarState = -1;
    mOldFreeModeKeyState = -1;
    mBoundingVolume = new BoundingSphere(position, 20.0);
}

FirstPersonCamera::~FirstPersonCamera() {}

void FirstPersonCamera::Update(float dt)
{
    // Prevent from having the camera move only when the cursor is within the windows
	EventManager::DisableMouseCursor();

	// The Camera moves based on the User inputs
	// - You can access the mouse motion with EventManager::GetMouseMotionXY()
	// - For mapping A S D W, you can look in World.cpp for an example of accessing key states
	// - Don't forget to use dt to control the speed of the camera motion

	// Mouse motion to get the variation in angle
	mHorizontalAngle -= EventManager::GetMouseMotionX() * mAngularSpeed * dt;
	mVerticalAngle   -= EventManager::GetMouseMotionY() * mAngularSpeed * dt;

	// Clamp vertical angle to [-85, 85] degrees
	mVerticalAngle = std::max(-85.0f, std::min(85.0f, mVerticalAngle));
	if (mHorizontalAngle > 360)
	{
		mHorizontalAngle -= 360;
	}
	else if (mHorizontalAngle < -360)
	{
		mHorizontalAngle += 360;
	}

	float theta = radians(mHorizontalAngle);
	float phi = radians(mVerticalAngle);

	mLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));

    int currentFreeModeKeyState = glfwGetKey(EventManager::GetWindow(), GLFW_KEY_F );
    if (currentFreeModeKeyState == GLFW_PRESS)
    {
        if (currentFreeModeKeyState != mOldFreeModeKeyState)
        {
            mFreeMode = !mFreeMode;
            if (mFreeMode)
            {
				mSpeed = 115;
                printf("Flying Mode enabled!\n");
            }
            else
            {
				mSpeed = 45;
                printf("Flying Mode disabled!\n");
            }
        }
    }
    mOldFreeModeKeyState = currentFreeModeKeyState;

    if (!mFreeMode)
    {
        glm::vec3 newPosition = mPosition + computeMovement(dt);

        if (!World::CheckCollisions(newPosition.x, newPosition.z, mBoundingVolume))
        {
            mPosition = newPosition;
        }
        else
        {
            printf("Collision found at (%f, %f)", newPosition.x, newPosition.z);
        }
        int currentSpaceBarState = glfwGetKey(EventManager::GetWindow(), GLFW_KEY_SPACE);
        if (currentSpaceBarState == GLFW_PRESS && !mJumping)
        {
            if (currentSpaceBarState != mOldSpaceBarState)
            {
                mJumping = true;
                mVelocity = JUMP_FORCE;
            }
        }
        else
        {
            mVelocity += GRAVITY * dt;
        }
        mOldSpaceBarState = currentSpaceBarState;


        float newHeight = computeHeight(dt);
        mPosition.y = newHeight;
        mPreviousHeight = newHeight;

    }
    else
    {
        // Uncomment for debugging
        //World::CheckCollisions(mPosition.x, mPosition.z, mBoundingVolume);
        mPosition += computeMovement(dt);
    }

    if (mBoundingVolume != nullptr)
    {
        mBoundingVolume->SetPosition(mPosition);
    }
}

glm::vec3 FirstPersonCamera::computeMovement(float dt)
{
    glm::vec3 movement(0.0f, 0.0f, 0.0f);
    vec3 sideVector = glm::cross(mLookAt, vec3(0.0f, 1.0f, 0.0f));
    glm::normalize(sideVector);

    // A S D W for motion along the camera basis vectors
    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_W ) == GLFW_PRESS)
    {
        movement += mLookAt * dt * mSpeed;
    }

    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_S ) == GLFW_PRESS)
    {
        movement -= mLookAt * dt * mSpeed;
    }

    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_D ) == GLFW_PRESS)
    {
        movement += sideVector * dt * mSpeed;
    }

    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_A ) == GLFW_PRESS)
    {
        movement -= sideVector * dt * mSpeed;
    }

    return movement;
}

float FirstPersonCamera::computeHeight(float dt)
{
    auto *terrain = World::GetInstance()->GetTerrain();
    float currentHeight = terrain->GetHeightAt(mPosition.x, mPosition.z);
    float delta = glm::clamp(dt * CAMERA_RESPONSIVENESS, 0.0f, 1.0f);
    float finalHeight = glm::mix(mPreviousHeight, (BASE_HEIGHT + currentHeight) + (mVelocity * dt), delta);
    if (finalHeight < BASE_HEIGHT + currentHeight)
    {
        mVelocity = 0;
        mJumping = false;
        return glm::mix(mPreviousHeight, BASE_HEIGHT + currentHeight, delta);
    }
    return finalHeight;
}

glm::mat4 FirstPersonCamera::GetViewMatrix() const
{
	return glm::lookAt(	mPosition, mPosition + mLookAt, vec3(0.0f, 1.0f, 0.0f) );
}
