#pragma once
#include "JGCore.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/GraphicsDefine.h"
#include "Class/Asset/Asset.h"
#include "Class/SystemLayer.h"

namespace JG
{

	
	
	// Global Shader Setting
	// Sampler 추가
	// 오브젝트 인스턴스, 카메라 정보는 무조건 들어있음
	

	class IMesh;
	class IVertexBuffer;
	class IIndexBuffer;
	class IMaterial;
	class IRenderer;
	class Render2DBatch;
	class GraphicsSystemLayer : public ISystemLayer
	{
		class CameraItem
		{
		public:
			Camera* pCamera = nullptr;
		
			JVector2 Resolution;
			bool IsResizeDirty = false;
			List<SharedPtr<ITexture>> TargetTextures;
			List<SharedPtr<ITexture>> TargetDepthTextures;
			u64 CurrentIndex = 0;

			SharedPtr<IRenderer>  Renderer;
			SharedPtr<Render2DBatch> _2DBatch;
			Queue<SharedPtr<IRenderer>> PendingRendererQueue;


			CameraItem() = default;
			CameraItem(Camera* cam);

			bool IsValid() const;
			void ChangeRenderer();
		};
		enum class ERenderItemPriority
		{
			_2D,
		};
	private:
		UniquePtr<CameraItem> mMainCamera;
		Dictionary<Camera*, UniquePtr<CameraItem>> mRegisteredRenderCameras;

		Dictionary<Type, u64> mRenderItemPriority;
		SortedDictionary<u64, Dictionary<Type, List<SharedPtr<IRenderItem>>>> mPushedRenderItems;

		//SharedPtr<IMaterial> mStandardDefaultMaterial;

		//bool mIsRenderCompelete = false;
		std::atomic_bool mIsRenderCompelete = false;
	public:
		virtual ~GraphicsSystemLayer() {}
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Begin() override;
		virtual void Destroy() override;

		virtual void OnEvent(IEvent& e) override;
		virtual String GetLayerName() override;
	private:
		bool ResponsePushRenderItem(RequestPushRenderItemEvent& e);

		bool ResponseRegisterCamera(RequestRegisterCameraEvent& e);
		bool ResponseUnRegisterCamera(RequestUnRegisterCameraEvent& e);

		bool ResponseRegisterMainCamera(RequestRegisterMainCameraEvent& e);
		bool ResponseUnRegisterMainCamera(RequestUnRegisterMainCameraEvent& e);
	private:
		void Rendering(CameraItem* cameraItem, Type type, const List<SharedPtr<IRenderItem>>& renderItemList);
	private:
		EScheduleResult Update();
		EScheduleResult Wait();
	private:
		void LoadShaderScript();
		
	};
}