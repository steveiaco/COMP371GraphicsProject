// COMP 371 Assignment Framework
//
// Created by Emanuel Sharma with inspiration from the code of ThinMatrix: https://www.youtube.com/watch?v=HusvGeEDU_U
// Stores the model data for a water surface
//
#include "WaterQuad.h"

#include <GL/glew.h>

namespace pg
{
	namespace water
	{
		WaterQuad::WaterQuad()
		{
		}

		WaterQuad::WaterQuad(const float xPos, const float yPos, const float zPos, const float mWidth, const float mHeight)
			: mXPos(xPos)
			, mYPos(yPos)
			, mZPos(zPos)
			, mWidth(mWidth)
			, mHeight(mHeight)
		{
			GenVertexBuffer();
		}

		WaterQuad::WaterQuad(const WaterQuad& orig)
			: WaterQuad(orig.mXPos, orig.mYPos, orig.mZPos, orig.mWidth, orig.mHeight)
		{
		}

		WaterQuad::~WaterQuad()
		{
			// Free the GPU from the Vertex Buffer
			glDeleteBuffers(1, &mVBO);
			glDeleteVertexArrays(1, &mVAO);
		}

		glm::vec3 WaterQuad::GetPosition() const
		{
			return { mXPos, mYPos, mZPos };
		}

		glm::vec2 WaterQuad::GetSize() const
		{
			return { mWidth, mHeight };
		}

		void WaterQuad::GenVertexBuffer()
		{
			// Calculate vertex positions (we will use a triangle fan for efficiency)
			float vertices[] =
			{
				mXPos, mYPos, mZPos,
				mXPos, mYPos, mZPos + mHeight,
				mXPos + mWidth, mYPos, mZPos + mHeight,
				mXPos + mWidth, mYPos, mZPos,
			};

			// Create a vertex array
			glGenVertexArrays(1, &mVAO);
			glBindVertexArray(mVAO);

			// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
			glGenBuffers(1, &mVBO);
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// 1st attribute buffer : vertex Positions
			glVertexAttribPointer(
				0,					// attribute
				3,					// size
				GL_FLOAT,			// type
				GL_FALSE,			// normalized?
				3 * sizeof(float),	// stride
				(void*) 0			// array buffer offset
			);
			glEnableVertexAttribArray(0);
		}
	}
}