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
const std::vector<std::string> skyboxFaces{
"Textures/right.jpg",
"Textures/up.jpg",
"Textures/top.jpg",
"Textures/down.jpg",
"Textures/front.jpg",
"Textures/back.jpg"
};
#else
const std::vector<std::string> skyboxFaces{
"../Assets/Textures/right.jpg",
"../Assets/Textures/left.jpg",
"../Assets/Textures/top.jpg",
"../Assets/Textures/bottom.jpg",
"../Assets/Textures/front.jpg",
"../Assets/Textures/back.jpg"
};
#endif



Skybox::Skybox() {
	float SIZE = 50.0f;
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
    unsigned int texture;
    glGenTextures(1, &texture);
	assert(texture != 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    int width, height;
    for (unsigned int i = 0; i < skyboxFaces.size(); i++)
    {
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(skyboxFaces[i].c_str(), 0);
        FIBITMAP* image = FreeImage_Load(format, skyboxFaces[i].c_str());
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
            std::cout << "Could not load texture " << skyboxFaces[i].c_str() << std::endl;
        }
        FreeImage_Unload(image);
        FreeImage_Unload(image32bits);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
	mTexture = texture;
}

void Skybox::Draw() {
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_SKYBOX);
	glUseProgram(Renderer::GetShaderProgramID());
	glDepthMask(GL_FALSE);
	glBindVertexArray(mVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	Renderer::SetShader((ShaderType)prevShader);
}
