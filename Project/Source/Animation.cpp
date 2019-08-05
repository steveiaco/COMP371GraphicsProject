//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Animation.h"
#include "Renderer.h"
#include "World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;
using namespace std;

AnimationKey::AnimationKey() : Model()
{
}

AnimationKey::~AnimationKey()
{
}

void AnimationKey::Update(float dt)
{
	Model::Update(dt);
}

void AnimationKey::Draw()
{
	Model::Draw();
}

bool AnimationKey::ParseLine(const std::vector<ci_string> &token)
{
	if (token.empty())
	{
		return true;
	}
	else
	{
		return Model::ParseLine(token);
	}
}

Animation::Animation() 
	: mName(""), mCurrentTime(0.0f), mDuration(0.0f), mVBO(0), mVAO(0)
{
}

Animation::~Animation()
{
}

void Animation::CreateVertexBuffer()
{
    // This is just to display lines between the animation keys
    for (int i=0; i<(int)mKey.size(); ++i)
    {
        Vertex v;
        v.position = mKey[i].mPosition;
        mVertexBuffer.push_back(v);
    }
    
	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mVertexBuffer.size(), &(mVertexBuffer[0]), GL_STATIC_DRAW);
    
    // Create a vertex array
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    
    // Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mVertexBuffer.size(), &(mVertexBuffer[0]), GL_STATIC_DRAW);
    
    // 1st attribute buffer : vertex Positions
    glVertexAttribPointer(0,
                          3,                // size
                          GL_FLOAT,        // type
                          GL_FALSE,        // normalized?
                          sizeof(Vertex), // stride
                          (void*)0        // array buffer offset
                          );
    glEnableVertexAttribArray(0);

}

void Animation::Update(float dt)
{
	mCurrentTime += dt;

    while(mCurrentTime > mDuration)
    {
        mCurrentTime -= mDuration;
    }
}

void Animation::Draw()
{
	// Draw the Vertex Buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &glm::mat4(1.0)[0][0]); //No need for world tranform because positions are listed as world positions. Ergo, we can use an identity matrix.

	glDrawArrays(GL_LINE_LOOP, 0, mVertexBuffer.size());
}


void Animation::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while(std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		istream_iterator<ci_string, char, ci_char_traits> end;
		vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}


bool Animation::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty())
    {
        return true;
    }
	else if (token[0] == "name")
	{
		assert(token.size() > 2);
		assert(token[1] == "=");

		mName = token[2];
		return true;
	}
	else if (token[0] == "key")
	{
		assert(token.size() > 5);
        assert(token[1] == "=");
        assert(token[3] == "time");
        assert(token[4] == "=");

		ci_string name = token[2];
		AnimationKey *key = World::GetInstance()->FindAnimationKey(name);
        
		assert(key != nullptr);
		AddKey(key, (float) atof(token[5].c_str()));
		return true;
	}
	return false;
}

void Animation::AddKey(AnimationKey* key, float time)
{
    assert(time >= mDuration);
	mKey.push_back(*key);
    mKeyTime.push_back(time);
    mDuration = time;
}

ci_string Animation::GetName() const
{
	return mName;
}

glm::mat4 Animation::GetAnimationWorldMatrix() const
{

	//Find next key frame, use it to also find previous key frame.
	uint crrtIndex = 0;
	while (mKeyTime[crrtIndex] < mCurrentTime) //We don't need to check for out of bounds conditions because mCurrentTime is bound to [0, mDuration] in update.
	{
		crrtIndex++;
	}
	const AnimationKey& nextFrame = mKey[crrtIndex];
	const float nextFrameTime = mKeyTime[crrtIndex];
	const AnimationKey& lastFrame = mKey[crrtIndex - 1]; // This index will always be valid because crrtIndex will never get through the loop without first incrementing to 1
	const float lastFrameTime = mKeyTime[crrtIndex - 1];


	//Linear interpolation between key frames
	float progress = (mCurrentTime - lastFrameTime) / (nextFrameTime - lastFrameTime);
	glm::vec3 interpolatedScale = glm::mix(lastFrame.GetScaling(), nextFrame.GetScaling(), progress);
	glm::vec3 interpolatedTranslation = glm::mix(lastFrame.GetPosition(), nextFrame.GetPosition(), progress);
	glm::quat interpolatedQuat = glm::slerp(glm::angleAxis(glm::radians(lastFrame.GetRotationAngle()), lastFrame.GetRotationAxis()), glm::angleAxis(glm::radians(nextFrame.GetRotationAngle()), nextFrame.GetRotationAxis()), progress);

	//Transformations
	mat4 rotationMatrix = glm::toMat4(interpolatedQuat);
	return glm::scale(glm::translate(glm::mat4(1.f), interpolatedTranslation) * rotationMatrix, interpolatedScale);
}
