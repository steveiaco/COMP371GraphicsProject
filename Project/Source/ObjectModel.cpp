#include "ObjectModel.h"
#include "Renderer.h"
#include "World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

ObjectModel::ObjectModel() : Model(), textured(false)
{

}

ObjectModel::~ObjectModel()
{
}

void ObjectModel::Update(float dt)
{
}

void ObjectModel::Draw()
{
	//TODO implement instancing in cases where the model is specified to be a randomly placed one

	unsigned int prevShader = Renderer::GetCurrentShader();
	if (textured) {
		//Renderer::SetShader(SHADER_TEXTURED);
		//glUseProgram(Renderer::GetShaderProgramID());

		Renderer::CheckForErrors();

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		//glUniform1i(glGetUniformLocation(Renderer::GetShaderProgramID(), "myTextureSampler"), 0);

		Renderer::CheckForErrors();
	}

	//Draw the vertex buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);


	
	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3);
	

	Renderer::SetShader((ShaderType)prevShader);
	glUseProgram(Renderer::GetShaderProgramID());

}

bool ObjectModel::ParseLine(const std::vector<ci_string>& token)
{
	if (token.empty())
	{
		return true;
	}
	//required, specified path in which to load object file
	//no space allowed due to the way tokens are separated
	//TODO support spaces
	else if (token[0] == "path")
	{
		assert(token.size() > 2);
		assert(token[1] == "=");
		
		ci_string path_ci = token[2].substr(1, token[2].length() - 2);

		//since ci_string::c_str() returns a const char * that is stack allocated, leaving the scope of this method will invalidate the contents that the pointer points to.
		//because of this, we copy the contents of the const char * returned to the heap
		const std::string::size_type size = path_ci.size();
		path = new char[size + 1];   //we need extra char for NUL
		memcpy(path, path_ci.c_str(), size + 1);

		LoadVertices(path);
	}

	//specify texture file
	else if (token[0] == "texture") 
	{
		assert(token.size() > 2);
		assert(token[1] == "=");

		ci_string texturePath_ci = token[2].substr(1, token[2].length() - 2);

		//since ci_string::c_str() returns a const char * that is stack allocated, leaving the scope of this method will invalidate the contents that the pointer points to.
		//because of this, we copy the contents of the const char * returned to the heap
		const std::string::size_type size = texturePath_ci.size();
		texturePath = new char[size + 1];   //we need extra char for NUL
		memcpy(texturePath, texturePath_ci.c_str(), size + 1);

		textured = true;

		TextureLoader t;
		textureID = t.LoadTexture(texturePath);

	}
	else
	{
		return Model::ParseLine(token);
	}

}

Vertex* ObjectModel::LoadVertices(const char * path)
{
	/*Read obj file*/

	// Using case-insensitive strings and streams for easier parsing
	std::ifstream input;
	input.open(path);

	// Invalid file
	if (input.fail())
	{
		fprintf(stderr, "Error loading file: %s\n", path);
		getchar();
		exit(-1);
	}

	std::string line;

	std::cout << "Loading model: " << path << ", this will take some time... ";

	//I am going to assume the Obj files are well formed (information is defined in the order of vertices, vertex normals, vertex texture coordinates)
	while (std::getline(input, line))
	{

		/* ignore comments and empty lines */
		if (line[0] == '#' || line.find_first_not_of(' ') == std::string::npos)
		{
			continue;
		}

		/* when a vertex descriptor is found */
		else if(line[0] == 'v' && line[1] == ' ')
		{
			std::vector<std::string> coords = split(line, ' ');
			
			float x = static_cast<float>(atof(coords[1].c_str()));
			float y = static_cast<float>(atof(coords[2].c_str()));
			float z = static_cast<float>(atof(coords[3].c_str()));

			vertices.push_back(glm::vec3(x, y, z));
		}	

		/* when a vertex normal is found*/
		else if(line[0] == 'v' && line[1] == 'n')
		{
			std::vector<std::string> coords = split(line, ' ');

			float x = static_cast<float>(atof(coords[1].c_str()));
			float y = static_cast<float>(atof(coords[2].c_str()));
			float z = static_cast<float>(atof(coords[3].c_str()));

			normals.push_back(glm::vec3(x, y, z));
		}		

		/* when a face is found */
		else if (line[0] == 'f')
		{
			Face f;

			std::vector<std::string> res = split(line, ' ');
			
			for (auto it = res.begin(); it != res.end(); it++) 
			{
				std::string str = *it;

				//check if the string is only whitespace, empty, or equal to f
				if (str.find_first_not_of(' ') != std::string::npos && !str.empty() && str[0] != 'f')
				{
					std::vector<std::string> info = split(str.c_str(), '/');

					//Push vertex
					f.vertices.push_back(static_cast<int>(atoi(info[0].c_str())));
					
					//Push texture coordinates , if provided 
					if (info.size() >= 2 && !info[1].empty())
						f.textures.push_back(static_cast<int>(atoi(info[1].c_str())));

					//Push normals , if provided 
					if (info.size() >= 3 && !info[2].empty())
						f.normals.push_back(static_cast<int>(atoi(info[2].c_str())));

				}
			}

			//if quads, convert to triangles
			if (f.vertices.size() == 4) 
			{
				std::vector<Face> tris = ConvertQuadToTris(f);

				for(auto it = tris.begin(); it != tris.end(); it++)
					faces.push_back(*it);
			}
			else 
			{
				faces.push_back(f);
			}
				
		}

		/* when a vertex texture coordinate is found */
		else if(line[0] == 'v' && line[1] == 't')
		{
			std::vector<std::string> res = split(line, ' ');


			float x = static_cast<float>(atof(res[1].c_str()));
			float y = res.size() >= 3 ? static_cast<float>(atof(res[2].c_str())) : 0.0f;
			float z = res.size() >= 4 ? static_cast<float>(atof(res[3].c_str())) : 0.0f;

			textureCoordinates.push_back(glm::vec3(x, y, z));
		}
	}


	std::cout << "Done with " << vertices.size() << " vertices, " << faces.size() << " faces.\n";

	input.close();

	/* Read texture file (if exists) */




	/* Upload vertices to GPU */

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	std::vector<Vertex> verts = GenerateOrderedVertexList();
	Vertex* vertexArray = &verts[0];

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * faces.size() * 3, vertexArray, GL_STATIC_DRAW);

	// 1st attribute buffer : vertex positions
	glVertexAttribPointer(	0,						// attribute. No particular reason for 0, but must match the layout in the shader.
							3,						// size
							GL_FLOAT,				// type
							GL_FALSE,				// normalized?
							sizeof(Vertex),			// stride
							(void*)0				// array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normals
	glVertexAttribPointer(	1,					
							3,							// size
							GL_FLOAT,					// type	
							GL_FALSE,					// normalized
							sizeof(Vertex),				// stride size
							(void*)sizeof(glm::vec3)	//array buffer offset
	);
	glEnableVertexAttribArray(1);

	// 3rd attribute buffer : vertex colors (temporary)
	glVertexAttribPointer(	2,
							4,								// size
							GL_FLOAT,						// type	
							GL_FALSE,						// normalized
							sizeof(Vertex),					// stride size
							(void*)(2 * sizeof(glm::vec3))		//array buffer offset
	);
	glEnableVertexAttribArray(2);

	// 4th attribute buffer : texture coordinates
	glVertexAttribPointer(	3,
							2,
							GL_FLOAT,
							GL_FALSE,
							sizeof(Vertex),
							(void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec4)) // texture coordinates are Offseted by 2 vec3 (see class Vertex) and a vec4
	);
	glEnableVertexAttribArray(3);

	//TODO add attribute for texture coordinates?
}


//TAKEN FROM https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
//todo write in house split function
//internal split function
std::vector<std::string> ObjectModel::split(const std::string &s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

std::vector<ObjectModel::Vertex> ObjectModel::GenerateOrderedVertexList()
{
	std::vector<Vertex> vertexList;

	for (auto face = faces.begin(); face != faces.end(); face++)
	{
		for (int i = 0; i < face->vertices.size(); i++)
		{
			Vertex v;

			v.position = vertices[face->vertices[i] - 1];
			v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			if (textureCoordinates.size() != 0 && textured)
				v.texture = textureCoordinates[face->textures[i] - 1];

			if(normals.size() != 0)
				v.normal = normals[face->normals[i] - 1];

			

			vertexList.push_back(v);
		}
	}


	return vertexList;
}

std::vector<ObjectModel::Face> ObjectModel::ConvertQuadToTris(Face quad)
{
	std::vector<ObjectModel::Face> triangles;

	Face triangle1;
	Face triangle2;

	//generate triangle 1
	triangle1.vertices.push_back(quad.vertices[0]);
	triangle1.vertices.push_back(quad.vertices[1]);
	triangle1.vertices.push_back(quad.vertices[2]);

	if (quad.normals.size() == 4)
	{
		triangle1.normals.push_back(quad.normals[0]);
		triangle1.normals.push_back(quad.normals[1]);
		triangle1.normals.push_back(quad.normals[2]);
	}

	if (quad.textures.size() == 4)
	{
		triangle1.textures.push_back(quad.textures[0]);
		triangle1.textures.push_back(quad.textures[1]);
		triangle1.textures.push_back(quad.textures[2]);
	}

	//generate triangle 2
	triangle2.vertices.push_back(quad.vertices[0]);
	triangle2.vertices.push_back(quad.vertices[2]);
	triangle2.vertices.push_back(quad.vertices[3]);

	if (quad.normals.size() == 4)
	{
		triangle2.normals.push_back(quad.normals[0]);
		triangle2.normals.push_back(quad.normals[2]);
		triangle2.normals.push_back(quad.normals[3]);
	}

	if (quad.textures.size() == 4)
	{
		triangle2.textures.push_back(quad.textures[0]);
		triangle2.textures.push_back(quad.textures[2]);
		triangle2.textures.push_back(quad.textures[3]);
	}

	triangles.push_back(triangle1);
	triangles.push_back(triangle2);

	return triangles;
}

