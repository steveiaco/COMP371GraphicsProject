#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace pg
{
	namespace water
	{
		class WaterFrameBuffers
		{
		public:
			WaterFrameBuffers();
			~WaterFrameBuffers();

			void InitializeReflectionBuffers();
			void InitializeRefractionBuffers();

			void UnbindCurrentFrameBuffer();
			void BindReflectionFrameBuffer();
			void BindRefractionFrameBuffer();

			unsigned int GetReflectionTexture() const;
			unsigned int GetRefractionTexture() const;

		private:
			// RESOLUTION
			const static int REFLECTION_WIDTH = 320;
			const static int REFLECTION_HEIGHT = 180;

			const static int REFRACTION_WIDTH = 1280;
			const static int REFRACTION_HEIGHT = 720;

			// REFLECTION
			unsigned int mReflectionFrameBuffer = 0;
			unsigned int mReflectionTexture = 0;
			unsigned int mReflectionDepthBuffer = 0;

			// REFRACTION
			unsigned int mRefractionFrameBuffer = 0;
			unsigned int mRefractionTexture = 0;
			unsigned int mRefractionDepthTexture = 0;
		};
	}
}
