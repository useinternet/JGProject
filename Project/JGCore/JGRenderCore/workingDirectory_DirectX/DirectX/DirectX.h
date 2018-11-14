#pragma once
#include"DxWindow.h"
#include"BufferSystem/JGBufferEnum.h"
namespace JGRC
{
	enum class EStateType
	{
		DepthState,
		RasterState,
		BlendState
	};
	class CORE_EXPORT DirectX
	{
		
		typedef std::map<HWND, DxWindow*>  DxWindowArray;
		typedef std::pair<HWND, DxWindow*> DxWindowPair;
		typedef std::unordered_map<JGRCObject*, std::shared_ptr<JGRCObject>>    RCObjectArray;
		typedef std::pair<JGRCObject*, std::shared_ptr<JGRCObject>> RCObjectPair;
	private:
		static DirectX* Instance;
	private:
		RCObjectArray m_usObjects;
		DxWindowArray m_mDxWindow;
		std::unique_ptr<class JGDevice>       m_Device;
		std::unique_ptr<class JGRenderState>  m_RenderState;
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
		void Draw();
	public:
		bool Init();
		DxWindow* AddDxWindow(const DxWinConfig& config);
		void      DeleteDxWindow(HWND hWnd);
		DxWindow* GetWindow(HWND hWnd);
		template<typename ObjType>
		ObjType* CreateObject();
		void DeleteObject(JGRCObject* obj);
	public:
		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetContext() const;
	public:
		void SetDefaultDirectState();
		void SetDirectState(const EStateType StateType, const uint ConfigType);
		void SetBlendFactor(const real a1, const real a2, const real a3, const real a4);
		void SetSampleMask(const uint mask);
	};
}
template<typename ObjType>
ObjType* JGRC::DirectX::CreateObject()
{
	std::unique_ptr<ObjType> obj = std::make_unique<ObjType>();
	ObjType* result = obj.get();
	m_usObjects.insert(JGRC::DirectX::RCObjectPair(result,move(obj)));

	return result;
}
