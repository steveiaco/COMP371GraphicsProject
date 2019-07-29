//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//         with help from Jordan Rutty
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>

class Model;

class ParticleEmitter
{
public:
    ParticleEmitter(glm::vec3 position, const Model* parent = nullptr);
    virtual glm::vec3 GetPosition(); // Would be random for line / circle emitters
    
private:
    const Model* mpParent;
    glm::vec3 mPosition;
};
