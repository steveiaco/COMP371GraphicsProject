#include "ChunkObjectInstance.h"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer.h"
#include "World.h"
#include "Camera.h"
#include "World/Collisions/BoundingVolume.h"
#include "World/Collisions/EmptyVolume.h"
#include "World/Collisions/BoundingSphere.h"
#include "World/Collisions/BoundingBox.h"
#include <typeinfo>


unsigned int ChunkObjectInstance::currentVAO;
unsigned int ChunkObjectInstance::currentVBO;


ChunkObjectInstance::ChunkObjectInstance(ChunkObject * c) : model(c) 
{ 
	mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	mScaling = glm::vec3(0.0f, 0.0f, 0.0f);
	mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	InitBoundingVolume();
}

void ChunkObjectInstance::InitBoundingVolume()
{
    if (!BoundingVolume::IsValid(model->mBoundingVolume))
    {
        mBoundingVolume = BoundingVolume::InitializeVolume();
    }
    else
    {
        mBoundingVolume = model->mBoundingVolume->Clone();
    }
}

void ChunkObjectInstance::SetPosition(glm::vec3 position)
{
	mPosition = position;
    if (mBoundingVolume != nullptr)
    {
        mBoundingVolume->SetPosition(mPosition);
    }
}

void ChunkObjectInstance::SetScaling(glm::vec3 scaling)
{
	mScaling = scaling;
    if (mBoundingVolume != nullptr)
    {
        mBoundingVolume->SetScaling(mScaling);
    }
}

void ChunkObjectInstance::SetRotation(glm::vec3 angles)
{
	mRotation = angles;
}

glm::mat4 ChunkObjectInstance::GetWorldMatrix() const
{
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), mPosition);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), mScaling);
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	return translation * rotateX * rotateY * rotateZ * scale;
}

bool ChunkObjectInstance::CheckCollision(BoundingVolume *volume)
{
    if (mBoundingVolume != nullptr && volume != nullptr)
    {
        return mBoundingVolume->IsInVolume(volume);
    }
    printf("Both are null...\n");
    return false; // Default to no collision
}

void ChunkObjectInstance::Draw()
{
    // Make sure the BoundingVolume is updated with the instance
    if (mBoundingVolume != nullptr)
    {
        mBoundingVolume->SetPosition(mPosition);
        mBoundingVolume->SetScaling(mScaling);
        mBoundingVolume->SetRotation(mRotation);
    }

	//Draw the vertex buffer

	glBindVertexArray(model->mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, model->mVBO);

	//todo optimize this

	//prevent a rebind in the case that the same VAO or VBO is already bound
	//if (currentVAO != model->mVAO) 
	//{
	//	glBindVertexArray(model->mVAO);
	//	ChunkObjectInstance::currentVAO = model->mVAO;
	//}

	//if (currentVBO != model->mVBO) 
	//{
	//	glBindBuffer(GL_ARRAY_BUFFER, model->mVBO);
	//	ChunkObjectInstance::currentVBO = model->mVBO;
	//}

	//todo may be more efficient to do this in the draw call in TerrainChunk

	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_OBJECT_COLOR);
	glUseProgram(Renderer::GetShaderProgramID());

	//send the world transform to the uniform variable in the shader
	World* worldInstance = World::GetInstance();

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	GLuint ViewTransformLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
	glUniformMatrix4fv(ViewTransformLocation, 1, GL_FALSE, &(worldInstance->GetCurrentCamera()->GetViewMatrix())[0][0]);

	GLuint ProjectionTransformLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
	glUniformMatrix4fv(ProjectionTransformLocation, 1, GL_FALSE, &(worldInstance->GetCurrentCamera()->GetProjectionMatrix())[0][0]);
	
	worldInstance->SetLights();


	for (int i = 0; i < model->materials.size(); i++) 
	{
		char sUniformName[32];

		snprintf(sUniformName, 32, "diffuseCoefficient[%i]", i);
		GLuint diffuseLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);
		snprintf(sUniformName, 32, "ambientCoefficient[%i]", i);
		GLuint ambientLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);
		snprintf(sUniformName, 32, "specularColor[%i]", i);
		GLuint specularColorLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);
		snprintf(sUniformName, 32, "specularExponent[%i]", i);
		GLuint specularExponentLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);
		snprintf(sUniformName, 32, "alpha[%i]", i);
		GLuint alphaLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), sUniformName);

		glUniform3fv(diffuseLocation, 1, &model->materials[i]->diffuseCoefficient[0]);
		glUniform3fv(ambientLocation, 1, &model->materials[i]->ambientCoefficient[0]);
		glUniform3fv(specularColorLocation, 1, &model->materials[i]->specularColor[0]);
		glUniform1f(specularExponentLocation, model->materials[i]->specularExponent);
		glUniform1f(alphaLocation, model->materials[i]->alpha);


	}

	//draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, model->faces.size() * 3);


	// Restore previous shader
	Renderer::SetShader((ShaderType)prevShader);
	glUseProgram(Renderer::GetShaderProgramID());


}
