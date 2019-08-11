#include "WaterRenderer.h"
#include "WaterQuad.h"

#include "../../World.h"
#include "../../Renderer.h"
#include "../../Camera.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace pg
{
	namespace water
	{

		WaterRenderer::WaterRenderer(const ShaderType shaderID, const WaterFrameBuffers& FBOs)
			: mShaderID(shaderID)
			, mFBOs(FBOs)
		{
		}

		WaterRenderer::WaterRenderer(const WaterRenderer& orig)
			: mShaderID(orig.mShaderID)
			, mFBOs(orig.mFBOs)
		{
		}

		WaterRenderer::~WaterRenderer()
		{
			if (mIsRunning)
			{
				Stop();
			}
		}

		void WaterRenderer::Start()
		{
			mIsRunning = true;
			mPrevShaderID = static_cast <ShaderType> (Renderer::GetCurrentShader());
			Renderer::SetShader(mShaderID);
			glUseProgram(Renderer::GetShaderProgramID());

			const Camera* const pCamera = World::GetInstance()->GetCurrentCamera();

			// Send the view constants to the shader
			GLuint VMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
			glm::mat4 V = pCamera->GetViewMatrix();
			glUniformMatrix4fv(VMatrixLocation, 1, GL_FALSE, &V[0][0]);

			// Send the projection constants to the shader
			GLuint PMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
			glm::mat4 P = pCamera->GetProjectionMatrix();
			glUniformMatrix4fv(PMatrixLocation, 1, GL_FALSE, &P[0][0]);

			// Send reflection and refraction textures
			GLuint reflectionTextureLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "reflectionTexture");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mFBOs.GetReflectionTexture());
			glUniform1i(reflectionTextureLocation, 0);
			GLuint refractionTextureLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "refractionTexture");
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mFBOs.GetRefractionTexture());
			glUniform1i(refractionTextureLocation, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		void WaterRenderer::Draw(const WaterQuad& waterQuad)
		{
			// Bind data
			glBindVertexArray(waterQuad.GetVAO());
			glBindBuffer(GL_ARRAY_BUFFER, waterQuad.GetVBO());

			// Draw the water
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		void WaterRenderer::Stop()
		{
			glDisable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
			Renderer::SetShader(mPrevShaderID);
			glUseProgram(Renderer::GetShaderProgramID());
			mIsRunning = false;
		}
	}
}