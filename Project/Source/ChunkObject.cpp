#include "ChunkObject.h"
#include <istream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include "Renderer.h"

ChunkObject::ChunkObject()
{
	minTemperature = 0.0f;
	maxTemperature = 1.0f;
	
	minHumidity = 0.0f;
	maxHumidity = 1.0f;
	
	minAltitude = 0.0f;
	maxAltitude = 1.0f;

	minScaling = glm::vec3(0, 0, 0);
	maxScaling = glm::vec3(1, 1, 1);

	minRotation = glm::vec3(0, 0, 0);
	maxRotation = glm::vec3(360, 360, 360);
}

ChunkObject::~ChunkObject()
{
}

void ChunkObject::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while (std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		std::istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		std::istream_iterator<ci_string, char, ci_char_traits> end;
		std::vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}

bool ChunkObject::ParseLine(const std::vector<ci_string>& token)
{
	if (token.empty() == false)
	{
		if (token[0].empty() == false && token[0][0] == '#')
		{
			return true;
		}
		else if (token[0] == "name")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			mName = token[2];
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

			LoadOBJ(path);
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

			TextureLoader t;
			textureID = t.LoadTexture(texturePath);

		}
		else if (token[0] == "scaling_min") 
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			minScaling.x = static_cast<float>(atof(token[2].c_str()));
			minScaling.y = static_cast<float>(atof(token[3].c_str()));
			minScaling.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "scaling_max")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			minScaling.x = static_cast<float>(atof(token[2].c_str()));
			minScaling.y = static_cast<float>(atof(token[3].c_str()));
			minScaling.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "rotation_angle_min")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			minRotation.x = static_cast<float>(atof(token[2].c_str()));
			minRotation.y = static_cast<float>(atof(token[3].c_str()));
			minRotation.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "rotation_angle_max")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			maxRotation.x = static_cast<float>(atof(token[2].c_str()));
			maxRotation.y = static_cast<float>(atof(token[3].c_str()));
			maxRotation.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "temperature_range") 
		{
			assert(token.size() > 3);
			assert(token[1] == "=");

			minTemperature = static_cast<float>(atof(token[2].c_str()));
			maxTemperature = static_cast<float>(atof(token[3].c_str()));
		}
		else if (token[0] == "humidity_range") 
		{
			assert(token.size() > 3);
			assert(token[1] == "=");

			minHumidity = static_cast<float>(atof(token[2].c_str()));
			maxHumidity = static_cast<float>(atof(token[3].c_str()));
		}
		else if (token[0] == "altitude_range") 
		{
			assert(token.size() > 3);
			assert(token[1] == "=");

			minAltitude = static_cast<float>(atof(token[2].c_str()));
			maxAltitude = static_cast<float>(atof(token[3].c_str()));
		}
		else
		{
			return false;
		}
	}

	return true;
}

void ChunkObject::LoadOBJ(const char * path)
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
		else if (line[0] == 'v' && line[1] == ' ')
		{
			std::vector<std::string> coords = split(line, ' ');

			float x = static_cast<float>(atof(coords[1].c_str()));
			float y = static_cast<float>(atof(coords[2].c_str()));
			float z = static_cast<float>(atof(coords[3].c_str()));

			vertices.push_back(glm::vec3(x, y, z));
		}

		/* when a vertex normal is found*/
		else if (line[0] == 'v' && line[1] == 'n')
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

				for (auto it = tris.begin(); it != tris.end(); it++)
					faces.push_back(*it);
			}
			else
			{
				faces.push_back(f);
			}

		}

		/* when a vertex texture coordinate is found */
		else if (line[0] == 'v' && line[1] == 't')
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
	glVertexAttribPointer(0,						// attribute. No particular reason for 0, but must match the layout in the shader.
		3,						// size
		GL_FLOAT,				// type
		GL_FALSE,				// normalized?
		sizeof(Vertex),			// stride
		(void*)0				// array buffer offset
	);
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : vertex normals
	glVertexAttribPointer(1,
		3,							// size
		GL_FLOAT,					// type	
		GL_FALSE,					// normalized
		sizeof(Vertex),				// stride size
		(void*)sizeof(glm::vec3)	//array buffer offset
	);
	glEnableVertexAttribArray(1);

	// 3rd attribute buffer : vertex colors (temporary)
	glVertexAttribPointer(2,
		4,								// size
		GL_FLOAT,						// type	
		GL_FALSE,						// normalized
		sizeof(Vertex),					// stride size
		(void*)(2 * sizeof(glm::vec3))		//array buffer offset
	);
	glEnableVertexAttribArray(2);

	// 4th attribute buffer : texture coordinates
	glVertexAttribPointer(3,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec4)) // texture coordinates are Offseted by 2 vec3 (see class Vertex) and a vec4
	);
	glEnableVertexAttribArray(3);
}

std::vector<ChunkObject::Vertex> ChunkObject::GenerateOrderedVertexList()
{
	std::vector<Vertex> vertexList;

	for (auto face = faces.begin(); face != faces.end(); face++)
	{
		for (int i = 0; i < face->vertices.size(); i++)
		{
			Vertex v;

			v.position = vertices[face->vertices[i] - 1];
			v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			if (textureCoordinates.size() != 0)
				v.texture = textureCoordinates[face->textures[i] - 1];

			if (normals.size() != 0)
				v.normal = normals[face->normals[i] - 1];



			vertexList.push_back(v);
		}
	}


	return vertexList;
}

std::vector<ChunkObject::Face> ChunkObject::ConvertQuadToTris(Face quad)
{
	std::vector<ChunkObject::Face> triangles;

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


//TAKEN FROM https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
//todo write in house split function
//internal split function
std::vector<std::string> ChunkObject::split(const std::string &s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}