#pragma once

#include "WaterFrameBuffers.h"

enum ShaderType;

namespace pg
{
	namespace water
	{
		class WaterQuad;

		class WaterRenderer
		{
		public:
			WaterRenderer() = delete;
			WaterRenderer(const ShaderType shaderID, const WaterFrameBuffers& fbos);
			WaterRenderer(const WaterRenderer& orig);

			~WaterRenderer();

			// CALL ORDER
			//1 - Call Start() first (sets up shader with universal data)
			//2 - Call Draw() for every WaterQuad to be drawn (loads up quad specific data and draws quads)
			//3 - Call Stop() when done (frees up any ressources being used by WaterRenderer and returns shader being used by renderer to original)
			void Start();
			void Draw(const WaterQuad& waterQuad);
			void Stop();

		private:
			bool mIsRunning = false;
			ShaderType mPrevShaderID;

			const ShaderType mShaderID;
			const WaterFrameBuffers& mFBOs;
		};
	}
}