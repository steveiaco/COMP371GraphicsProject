#version 330 core

// Input vertex data, only points for PathLines
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 ViewProjectionTransform;
uniform mat4 WorldTransform;

// Output to fragment shader
out vec4 v_color;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  ViewProjectionTransform * WorldTransform * vec4(vertexPosition_modelspace,1);
}

