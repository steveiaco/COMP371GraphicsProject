#include "WaterFrameBuffers.h"
#include "..\..\Renderer.h"

namespace pg
{
	namespace water
	{
		WaterFrameBuffers::WaterFrameBuffers()
		{
			InitializeReflectionBuffers();
			InitializeRefractionBuffers();
		}

		WaterFrameBuffers::~WaterFrameBuffers()
		{
			// REFLECTION
			glDeleteFramebuffers(1, &mReflectionFrameBuffer);
			glDeleteTextures(1, &mReflectionTexture);
			glDeleteRenderbuffers(1, &mReflectionDepthBuffer);
			// REFRACTION
			glDeleteFramebuffers(1, &mRefractionFrameBuffer);
			glDeleteTextures(1, &mRefractionTexture);
			glDeleteTextures(1, &mRefractionDepthTexture);
		}

		void WaterFrameBuffers::InitializeReflectionBuffers()
		{
			// Generate reflection frame buffer
			glGenFramebuffers(1, &mReflectionFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, mReflectionFrameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			// Generate reflection texture
			glGenTextures(1, &mReflectionTexture);
			glBindTexture(GL_TEXTURE_2D, mReflectionTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mReflectionTexture, 0);

			// Generate reflection depth buffer
			glGenTextures(1, &mReflectionDepthBuffer);
			glBindTexture(GL_TEXTURE_2D, mReflectionDepthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mReflectionDepthBuffer, 0);

			// Return binding to default frame buffer
			UnbindCurrentFrameBuffer();
		}

		void WaterFrameBuffers::InitializeRefractionBuffers()
		{
			// Generate refraction frame buffer
			glGenFramebuffers(1, &mRefractionFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, mRefractionFrameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			// Generate refraction texture
			glGenTextures(1, &mRefractionTexture);
			glBindTexture(GL_TEXTURE_2D, mRefractionTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRefractionTexture, 0);

			// Generate refraction depth texture
			glGenTextures(1, &mRefractionDepthTexture);
			glBindTexture(GL_TEXTURE_2D, mRefractionDepthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mRefractionDepthTexture, 0);

			// Return binding to default frame buffer
			UnbindCurrentFrameBuffer();
		}

		void WaterFrameBuffers::BindReflectionFrameBuffer()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, mReflectionFrameBuffer);
			glViewport(0, 0, REFLECTION_WIDTH, REFLECTION_HEIGHT);
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}

		void WaterFrameBuffers::BindRefractionFrameBuffer()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, mRefractionFrameBuffer);
			glViewport(0, 0, REFRACTION_WIDTH, REFRACTION_HEIGHT);
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}

		void WaterFrameBuffers::UnbindCurrentFrameBuffer()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, 1024, 768);
		}

		unsigned int WaterFrameBuffers::GetReflectionTexture() const
		{
			return mReflectionTexture;
		}

		unsigned int WaterFrameBuffers::GetRefractionTexture() const
		{
			return mRefractionTexture;
		}
	}
}