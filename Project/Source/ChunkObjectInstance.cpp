#include "ChunkObjectInstance.h"
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer.h"


unsigned int ChunkObjectInstance::currentVAO;
unsigned int ChunkObjectInstance::currentVBO;


ChunkObjectInstance::ChunkObjectInstance(ChunkObject * c) : model(c) 
{ 
	mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	mScaling = glm::vec3(0.0f, 0.0f, 0.0f);
	mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
}

void ChunkObjectInstance::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void ChunkObjectInstance::SetScaling(glm::vec3 scaling)
{
	mScaling = scaling;
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

void ChunkObjectInstance::Draw()
{
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

	//send the world transform to the uniform variable in the shader
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	//draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, model->faces.size() * 3);

	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
}
