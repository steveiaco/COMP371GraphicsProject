#include "LightSource.h"

using namespace glm;

LightSource::LightSource()
{
}

LightSource::LightSource(glm::vec4 position, glm::vec3 color)
{
	mPosition = position;
	mColor = color;
}

LightSource::~LightSource()
{
}

void LightSource::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while (std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		istream_iterator<ci_string, char, ci_char_traits> end;
		vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}

bool Model::ParseLine(const std::vector<ci_string> &token)
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
		else if (token[0] == "position")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mPosition.x = static_cast<float>(atof(token[2].c_str()));
			mPosition.y = static_cast<float>(atof(token[3].c_str()));
			mPosition.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "rotation")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mRotationAxis.x = static_cast<float>(atof(token[2].c_str()));
			mRotationAxis.y = static_cast<float>(atof(token[3].c_str()));
			mRotationAxis.z = static_cast<float>(atof(token[4].c_str()));
			mRotationAngleInDegrees = static_cast<float>(atof(token[5].c_str()));

			glm::normalize(mRotationAxis);
		}
		else if (token[0] == "scaling")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mScaling.x = static_cast<float>(atof(token[2].c_str()));
			mScaling.y = static_cast<float>(atof(token[3].c_str()));
			mScaling.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "animation")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			ci_string animName = token[2];

			mAnimation = World::GetInstance()->FindAnimation(animName);
		}
		else if (token[0] == "particleemitter")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			ParticleDescriptor* desc = World::GetInstance()->FindParticleDescriptor(token[2]);
			assert(desc != nullptr);

			ParticleEmitter* emitter = new ParticleEmitter(vec3(0.0f, 0.0f, 0.0f), this);

			ParticleSystem* ps = new ParticleSystem(emitter, desc);
			World::GetInstance()->AddParticleSystem(ps);
		}
		else if (token[0] == "materialcoefficients")
		{
			assert(token.size() > 5);
			assert(token[1] == "=");

			ka = static_cast<float>(atof(token[2].c_str()));
			kd = static_cast<float>(atof(token[3].c_str()));
			ks = static_cast<float>(atof(token[4].c_str()));
			n = static_cast<float>(atof(token[5].c_str()));
		}
		else
		{
			return false;
		}
	}

	return true;
}