//
//  Skybox.cpp
//  COMP371_Framework
//
//  Created by Emilio Assuncao on 8/6/19.
//  Copyright © 2019 Concordia. All rights reserved.
//

#include "Skybox.h"
#include "World.h"
#include "Renderer.h"

#include <iostream>
#include "TextureLoader.h"

#if defined(linux)
#include <FreeImage.h>
#else
#include <FreeImageIO.h>
#endif

using namespace glm;

#if defined(PLATFORM_OSX)
const std::vector<std::string> daySkyboxFaces{
"Textures/day_right.jpg",
"Textures/day_left.jpg",
"Textures/day_top.jpg",
"Textures/day_bottom.jpg",
"Textures/day_front.jpg",
"Textures/day_back.jpg"
};
const std::vector<std::string> nightSkyboxFaces{
"Textures/night_right.jpg",
"Textures/night_left.jpg",
"Textures/night_top.jpg",
"Textures/night_bottom.jpg",
"Textures/night_front.jpg",
"Textures/night_back.jpg"
};
#else
const std::vector<std::string> daySkyboxFaces{
"../Assets/Textures/day_right.jpg",
"../Assets/Textures/day_left.jpg",
"../Assets/Textures/day_top.jpg",
"../Assets/Textures/day_bottom.jpg",
"../Assets/Textures/day_front.jpg",
"../Assets/Textures/day_back.jpg"
};
const std::vector<std::string> nightSkyboxFaces{
"../Assets/Textures/night_right.jpg",
"../Assets/Textures/night_left.jpg",
"../Assets/Textures/night_top.jpg",
"../Assets/Textures/night_bottom.jpg",
"../Assets/Textures/night_front.jpg",
"../Assets/Textures/night_back.jpg"
};
#endif

Skybox::Skybox() {
	float SIZE = 1.0f;
	const Vertex skyboxVertices[]{
		// positions
		
		{glm::vec3(-SIZE,  SIZE, -SIZE)},
		{glm::vec3(-SIZE, -SIZE, -SIZE)},
		{glm::vec3(SIZE, -SIZE, -SIZE)},
		{glm::vec3(SIZE, -SIZE, -SIZE)},
		{glm::vec3(SIZE,  SIZE, -SIZE)},
		{glm::vec3(-SIZE,  SIZE, -SIZE)},

		{glm::vec3(-SIZE, -SIZE,  SIZE)},
		{glm::vec3(-SIZE, -SIZE, -SIZE)},
		{glm::vec3(-SIZE,  SIZE, -SIZE)},
		{glm::vec3(-SIZE,  SIZE, -SIZE)},
		{glm::vec3(-SIZE,  SIZE,  SIZE)},
		{glm::vec3(-SIZE, -SIZE,  SIZE)},

		{glm::vec3(SIZE, -SIZE, -SIZE)},
		{glm::vec3(SIZE, -SIZE,  SIZE)},
		{glm::vec3(SIZE,  SIZE,  SIZE)},
		{glm::vec3(SIZE,  SIZE,  SIZE)},
		{glm::vec3(SIZE,  SIZE, -SIZE)},
		{glm::vec3(SIZE, -SIZE, -SIZE)},

		{glm::vec3(-SIZE, -SIZE,  SIZE)},
		{glm::vec3(-SIZE,  SIZE,  SIZE)},
		{glm::vec3(SIZE,  SIZE,  SIZE)},
		{glm::vec3(SIZE,  SIZE,  SIZE)},
		{glm::vec3(SIZE, -SIZE,  SIZE)},
		{glm::vec3(-SIZE, -SIZE,  SIZE)},

		{glm::vec3(-SIZE,  SIZE, -SIZE)},
		{glm::vec3(SIZE,  SIZE, -SIZE)},
		{glm::vec3(SIZE,  SIZE,  SIZE)},
		{glm::vec3(SIZE,  SIZE,  SIZE)},
		{glm::vec3(-SIZE,  SIZE,  SIZE)},
		{glm::vec3(-SIZE,  SIZE, -SIZE)},

		{glm::vec3(-SIZE, -SIZE, -SIZE)},
		{glm::vec3(-SIZE, -SIZE,  SIZE)},
		{glm::vec3(SIZE, -SIZE, -SIZE)},
		{glm::vec3(SIZE, -SIZE, -SIZE)},
		{glm::vec3(-SIZE, -SIZE,  SIZE)},
		{glm::vec3(SIZE, -SIZE,  SIZE)},
	};
    loadSkybox();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)0);
    glEnableVertexAttribArray(0);
}

Skybox::~Skybox() {

}

void Skybox::loadSkybox() {
    glGenTextures(1, &mDayTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mDayTexture);
    
    int width, height;
    for (unsigned int i = 0; i < daySkyboxFaces.size(); i++)
    {
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(daySkyboxFaces[i].c_str(), 0);
        FIBITMAP* image = FreeImage_Load(format, daySkyboxFaces[i].c_str());
        FIBITMAP* image32bits = FreeImage_ConvertTo32Bits(image);
		FreeImage_FlipVertical(image);
        if (image)
        {
			
            width = FreeImage_GetWidth(image32bits);
            height = FreeImage_GetHeight(image32bits);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height,
				0, GL_BGR, GL_UNSIGNED_BYTE, (const void*)FreeImage_GetBits(image));
            
        }
        else {
            std::cout << "Could not load texture " << daySkyboxFaces[i].c_str() << std::endl;
        }
        FreeImage_Unload(image);
        FreeImage_Unload(image32bits);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void Skybox::Draw(int dayNightPhase, float dayNightRatio) {
	glDepthMask(GL_FALSE);
	glBindVertexArray(mVAO);
	GLuint dayNightPhaseLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "dayNightPhase");
	glUniform1i(dayNightPhaseLocation, dayNightPhase);
	GLuint dayNightRatioLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "dayNightRatio");
	glUniform1f(dayNightRatioLocation, dayNightRatio);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mDayTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}
