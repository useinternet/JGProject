#pragma once
#include "JGCore.h"
#include "Graphics/GraphicsDefine.h"


namespace JG
{
	class IFrameBuffer;
	class JGImGui : public GlobalSingleton<JGImGui>
	{
		friend class EditorUISystemLayer;
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