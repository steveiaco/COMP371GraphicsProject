//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//         with help from Jordan Rutty
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//
#include "ParticleDescriptor.h"

using namespace glm;
using namespace std;

ParticleDescriptor::ParticleDescriptor() :  name("unnamed"), velocity(), velocityAngleRandomness(0.0f), acceleration(),
                                            initialRotationAngle(0.0f), initialRotationAngleRandomness(0.0f),
                                            initialSize(100.0f, 100.0f), initialSizeRandomness(), sizeGrowthVelocity(0.0f),
                                            initialColor(1.0f,1.0f,1.0f,1.0f), midColor(1.0f,1.0f,1.0f,1.0f), endColor(1.0f,1.0f,1.0f,1.0f),
                                            emissionRate(1.0f), fadeInTime(1.0f), fadeOutTime(1.0f), totalLifetime(3.0f), totalLifetimeRandomness(0.0f)
{
}

bool ParticleDescriptor::ParseLine(vector<ci_string> token)
{
    if (token.empty())
    {
        return true;
    }
    if (token[0].empty() == false && token[0][0] == '#')
    {
        return true;
    }
    else if (token[0] == "name")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        name = token[2];
    }
    else if (token[0] == "velocity")
    {
        assert(token.size() > 4);
        assert(token[1] == "=");
        
        velocity.x = static_cast<float>(atof(token[2].c_str()));
        velocity.y = static_cast<float>(atof(token[3].c_str()));
        velocity.z = static_cast<float>(atof(token[4].c_str()));
    }
    else if (token[0] == "velocityAngleRandomness")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        velocityAngleRandomness = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "acceleration")
    {
        assert(token.size() > 4);
        assert(token[1] == "=");
        
        acceleration.x = static_cast<float>(atof(token[2].c_str()));
        acceleration.y = static_cast<float>(atof(token[3].c_str()));
        acceleration.z = static_cast<float>(atof(token[4].c_str()));
    }
    else if (token[0] == "initialRotationAngle")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        initialRotationAngle = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "initialRotationAngleRandomness")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        initialRotationAngleRandomness = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "initialSize")
    {
        assert(token.size() > 3);
        assert(token[1] == "=");
        
        initialSize.x = static_cast<float>(atof(token[2].c_str()));
        initialSize.y = static_cast<float>(atof(token[3].c_str()));
    }
    else if (token[0] == "initialSizeRandomness")
    {
        assert(token.size() > 3);
        assert(token[1] == "=");
        
        initialSizeRandomness.x = static_cast<float>(atof(token[2].c_str()));
        initialSizeRandomness.y = static_cast<float>(atof(token[3].c_str()));
    }
    else if (token[0] == "sizeGrowthVelocity")
    {
        assert(token.size() > 3);
        assert(token[1] == "=");
        
        sizeGrowthVelocity.x = static_cast<float>(atof(token[2].c_str()));
        sizeGrowthVelocity.y = static_cast<float>(atof(token[3].c_str()));
    }
    else if (token[0] == "initialColor")
    {
        assert(token.size() > 5);
        assert(token[1] == "=");
        
        initialColor.r = static_cast<float>(atof(token[2].c_str()));
        initialColor.g = static_cast<float>(atof(token[3].c_str()));
        initialColor.b = static_cast<float>(atof(token[4].c_str()));
        initialColor.a = static_cast<float>(atof(token[5].c_str()));
    }
    else if (token[0] == "midColor")
    {
        assert(token.size() > 5);
        assert(token[1] == "=");
        
        midColor.r = static_cast<float>(atof(token[2].c_str()));
        midColor.g = static_cast<float>(atof(token[3].c_str()));
        midColor.b = static_cast<float>(atof(token[4].c_str()));
        midColor.a = static_cast<float>(atof(token[5].c_str()));
    }
    else if (token[0] == "endColor")
    {
        assert(token.size() > 5);
        assert(token[1] == "=");
        
        endColor.r = static_cast<float>(atof(token[2].c_str()));
        endColor.g = static_cast<float>(atof(token[3].c_str()));
        endColor.b = static_cast<float>(atof(token[4].c_str()));
        endColor.a = static_cast<float>(atof(token[5].c_str()));
    }
    else if (token[0] == "emissionRate")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        emissionRate = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "fadeInTime")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        fadeInTime = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "fadeOutTime")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        fadeOutTime = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "totalLifetime")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        totalLifetime = static_cast<float>(atof(token[2].c_str()));
    }
    else if (token[0] == "totalLifetimeRandomness")
    {
        assert(token.size() > 2);
        assert(token[1] == "=");
        
        totalLifetimeRandomness = static_cast<float>(atof(token[2].c_str()));
    }
    else
    {
        fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
        getchar();
        exit(-1);
    }
    
    return true;
}

void ParticleDescriptor::Load(ci_istringstream& iss)
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
