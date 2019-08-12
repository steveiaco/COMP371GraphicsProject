// COMP 371 Assignment Framework
//
// Created by Emanuel Sharma with inspiration from the code of ThinMatrix: https://www.youtube.com/watch?v=HusvGeEDU_U
// Stores the model data for a water surface
//
#pragma once

#include <glm/glm.hpp>

namespace pg
{
	namespace water
	{
		class WaterQuad
		{
		public:
			WaterQuad();
			WaterQuad(const float xPos, const float yPos, const float zPos, const float mWidth, const float mHeight);
			WaterQuad(const WaterQuad& orig);

			~WaterQuad();

			glm::vec3 GetPosition() const;
			glm::vec2 GetSize() const;
			unsigned int GetVAO() const { return mVAO; };
			unsigned int GetVBO() const { return mVBO; };

			void GenVertexBuffer();

		private:
			unsigned int mVAO = 0;
			unsigned int mVBO = 0;

			const float mXPos = 0.f;
			const float mYPos = 0.f;
			const float mZPos = 0.f;

			const float mWidth = 10.f;
			const float mHeight = 10.f;
		};
	}
}