#include "ObjectModel.h"
#include "Renderer.h"

ObjectModel::ObjectModel() : Model()
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

	//Draw the vertex buffer
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices); 
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
		path = path_ci.c_str();

		LoadVertices(path);
	}
	//we'll define this as [0,1]
	else if (token[0] == "density") 
	{
		assert(token.size() > 2);
		assert(token[1] == "=");

		density = static_cast<float>(atof(token[2].c_str()));
	}
	else
	{
		return Model::ParseLine(token);
	}

}

Vertex* ObjectModel::LoadVertices(const char * path)
{

	/*Read obj file*/

	int numberVertex = 0;
	int numberVertexNormal = 0;
	int numberTextureCoordinates = 0;

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
		//TODO, seems like the regex detection of the obj files is very slow, i'll optimize this soon 
		std::regex commentRegex("\\s*#.*");
		std::regex vertexRegex("\\s*(v)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*");
		std::regex vertexNormalRegex("\\s*(vn)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*");
		std::regex vertexTextureCoordinateRegex("\\s*(vt)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*");
		std::regex faceRegex("\\s*(f)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*");

		std::match_results<const char *> results;
		//ignore comments
		if (std::regex_match(line, commentRegex)) 
		{
			continue;
		}
		//when a vertex descriptor is found
		else if(std::regex_match(line.c_str(), results, vertexRegex))
		{
			Vertex v;

			//TODO remove this once textures are fixed
			v.color = glm::vec3(0.0f, 0.0f, 1.0f);

			float x = static_cast<float>(atof(results[2].first));
			float y = static_cast<float>(atof(results[3].first));
			float z = static_cast<float>(atof(results[4].first));

			v.position = glm::vec3(x, y, z);

			vertices.push_back(v);

			numberVertex++;
		}		
		//when a vertex normal is found
		else if(std::regex_match(line.c_str(), results, vertexNormalRegex))
		{
			float x = static_cast<float>(atof(results[2].first));
			float y = static_cast<float>(atof(results[3].first));
			float z = static_cast<float>(atof(results[4].first));

			vertices[numberVertexNormal].normal = glm::vec3(x, y, z);

			numberVertexNormal++;
		}		

		//TODO implement face data reading

		//TODO find a way to store texture coordinates, they do not map 1:1 to vertices
		//when a vertex texture coordinate is found
		//else if(std::regex_match(line.c_str(), results, vertexTextureCoordinateRegex))
		//{
		//	float x = static_cast<float>(atof(results[2].first));
		//	float y = static_cast<float>(atof(results[3].first));
		//	float z = static_cast<float>(atof(results[4].first));

		//	vertices[numberTextureCoordinates].textureCoordinate = glm::vec3(x, y, z);

		//	numberTextureCoordinates++;
		//}
	}

	numberOfVertices = numberVertex;
	numberOfVertexNormals = numberVertexNormal;
	numberOfTextureCoordinates = numberTextureCoordinates;

	std::cout << "Done.";

	input.close();

	/* Upload vertices to GPU */

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	Vertex* vertexArray = &vertices[0];

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertexArray, GL_STATIC_DRAW);

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
							3,								// size
							GL_FLOAT,						// type	
							GL_FALSE,						// normalized
							sizeof(Vertex),					// stride size
							(void*)(2 * sizeof(glm::vec3))		//array buffer offset
	);
	glEnableVertexAttribArray(2);


	//TODO add attribute for texture coordinates?
}
