//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "ParsingHelper.h"
#include "Model.h"
#include <vector>

#include <glm/glm.hpp>


class AnimationKey : public Model
{
public:
    AnimationKey();
    ~AnimationKey();

	virtual void Update(float dt);
	virtual void Draw();
    
protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);
};

class Animation
{
public:
    
	Animation();
    virtual ~Animation();

	void CreateVertexBuffer();

    virtual void Update(float dt);
    virtual void Draw();

    void AddKey(AnimationKey* key, float time);
	glm::mat4 GetAnimationWorldMatrix() const;

	void Load(ci_istringstream& iss);
	ci_string GetName() const;

protected:
    virtual bool ParseLine(const std::vector<ci_string> &token);

private:
	ci_string mName; // The model name is mainly for debugging

	float mCurrentTime;
    float mDuration;

    std::vector<AnimationKey> mKey;
    std::vector<float> mKeyTime;

    // For drawing lines between keys
    // The vertex format is only position
    struct Vertex
    {
        glm::vec3 position;
    };
    
    std::vector<Vertex> mVertexBuffer;
    unsigned int mVAO;
    unsigned int mVBO;
};
