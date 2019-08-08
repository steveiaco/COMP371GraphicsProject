//
//  Skybox.hpp
//  COMP371_Framework
//
//  Created by Emilio Assuncao on 8/6/19.
//  Copyright © 2019 Concordia. All rights reserved.
//

#pragma once
#include <stdio.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>

class Skybox{
public:
    Skybox();
    ~Skybox();
    void loadSkybox();
    void Draw();
private:
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mTexture;

	struct Vertex
	{
		glm::vec3 position;
	};
};
