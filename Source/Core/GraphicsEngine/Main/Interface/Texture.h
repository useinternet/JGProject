#pragma once
#include "GEObject.h"


namespace GE
{
	class GRAPHICSENGINE_API TextureInfo
	{
	public:
		std::string path;
		int width = 0, height = 0;
		//std::string format;


	};
	class GRAPHICSENGINE_API Texture : public GEObject
	{
		
	public:
		virtual void* GetUserTextureData() const = 0;
		virtual const TextureInfo& GetInfo() const = 0;
		virtual bool  IsValid() const = 0;
		virtual ~Texture() {}
	};
}


