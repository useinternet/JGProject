#pragma once
#include"Dx12Include.h"
#include<memory>
#include<string>


namespace GR
{
	namespace Dx12
	{
		class Image
		{
		private:
			int m_Width;
			int m_Height;
			int m_Channels;
			bool m_Hdr;
			std::unique_ptr<unsigned char> m_Pixels;
		public:
			Image();
			static std::shared_ptr<Image> FromFile(const std::string& filename, int channels = 4);
		public:
		
			int Width()    const { return m_Width; }
			int Height()   const { return m_Height; }
			int Channels() const { return m_Channels; }
			int BytesPerPixel() const { return m_Channels * (m_Hdr ? sizeof(float) : sizeof(unsigned char)); }
			int Pitch() const { return m_Width * BytesPerPixel(); }
			long DataSize() const { return (long)Pitch() * (long)m_Height; }
			bool IsHDR() const { return m_Hdr; }

			template<typename T>
			const T* Pixels() const {
				return reinterpret_cast<const T*>(m_Pixels.get());
			}
		};
	}
}