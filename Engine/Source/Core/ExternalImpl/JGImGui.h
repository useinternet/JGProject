#pragma once
#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"


namespace JG
{
	class IFrameBuffer;
	class ImGuiSystemLayer;
	class JGImGui : public GlobalSingleton<JGImGui>
	{
		friend ImGuiSystemLayer;
	private:
		SharedPtr<IFrameBuffer> mFrameBuffer;
	public:
		JGImGui();
		~JGImGui();
	public:
		u64  ConvertImGuiTextureID(TextureID id);
		void NewFrame();
		void Resize(u32 width, u32 height);
	};

}