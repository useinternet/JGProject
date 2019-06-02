#include"pch.h"
#include"Image.h"
#include"stb_image.h"


namespace GR
{
	namespace Dx12
	{
		Image::Image() :
			m_Width(0),
			m_Height(0),
			m_Channels(0),
			m_Hdr(false)
		{ }
		std::shared_ptr<Image> Image::FromFile(const std::string& filename, int channels)
		{
			std::shared_ptr<Image> image = std::make_unique<Image>();

			if (stbi_is_hdr(filename.c_str()))
			{
				float* pixels = stbi_loadf(filename.c_str(), &image->m_Width, &image->m_Height, &image->m_Channels, channels);

				if (pixels)
				{
					image->m_Pixels.reset(reinterpret_cast<unsigned char*>(pixels));
					image->m_Hdr = true;
				}
			}
			else
			{
				unsigned char* pixels = stbi_load(filename.c_str(), &image->m_Width, &image->m_Height, &image->m_Channels, channels);
				if (pixels)
				{
					image->m_Pixels.reset(pixels);
					image->m_Hdr = false;
				}

			}

			if (channels > 0)
			{
				image->m_Channels = channels;
			}
			if (!image->m_Pixels)
				return nullptr;


			return image;
		}
	}
}


