#pragma once
#include"GraphicsPass.h"
namespace GR
{
	namespace Dx12
	{
		class Light;
		class RenderObject;
		class DepthFromLight : public GraphicsPass
		{
			static std::wstring m_ShaderVSPath;
			static std::wstring m_ShaderPSPath;
			static void SetShaderVSPath(const std::wstring& path);
			static void SetShaderPSPath(const std::wstring& path);
		private:
			RootSignature m_RootSignature;
			Viewport      m_Viewport;
			ScissorRect   m_ScissorRect;
			GraphicsPSO   m_VertexPSO;
			GraphicsPSO   m_SkinnedPSO;
			std::queue<RenderObject*> m_ObjectQueue;
			std::queue<Light*>   m_LightQueue;
		public:
			virtual void Init(uint32_t width, uint32_t height) override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Execute(GraphicsCommander* commander, const PassCB& passcb) override;
		public:
			void PushLight(Light* light);
			void PushObject(RenderObject* obj);
		private:
			void Execute_Spot_Directional(GraphicsCommander* commander, Light* light);
			void Execute_Point(GraphicsCommander* commander, Light* light);
		};
	}
}