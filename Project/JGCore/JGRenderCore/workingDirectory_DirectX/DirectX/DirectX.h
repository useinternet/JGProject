#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{
	class JGInitConfig;
	enum class EStateType
	{
		DepthState,
		RasterState,
		BlendState
	};
	class CORE_EXPORT DirectX
	{
	private:
		static DirectX* Instance;
	private:
		std::unique_ptr<class JGDevice>       m_Device;
		std::unique_ptr<class JGSwapChain>    m_SwapChain;
		std::unique_ptr<class JGRenderTarget> m_RenderTarget;
		std::unique_ptr<class JGRenderState>  m_RenderState;
		std::unique_ptr<class JGViewport>     m_Viewport;
		real m_BackColor[4];
		real m_BlendFactor[4];
		uint  m_SampleMask = 0xffffffff;
	private:
		DirectX();
		~DirectX();
		DirectX(const DirectX& copy) = delete;
		DirectX& operator=(const DirectX& dx) = delete;
	public:
		static DirectX* GetInstance();
		static void  Release();
	public:
		bool Init(const JGInitConfig& config);
		void BeginDraw();
		void EndDraw();
	public:
		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetContext() const;
		JGViewport* GetViewport() const;
	public:
		void SetDefaultDirectState();
		void SetBackColor(const real r, const real g, const real b, const real a);
		void SetDirectState(const EStateType StateType, const uint ConfigType);
		void SetBlendFactor(const real a1, const real a2, const real a3, const real a4);
		void SetSampleMask(const uint mask);
		void SetDefautRenderTarget();

	};
}
