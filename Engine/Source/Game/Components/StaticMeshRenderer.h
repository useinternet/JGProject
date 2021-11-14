#pragma once
#include "BaseRenderer.h"
namespace JG
{
	class StaticMeshRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		Asset<IMesh>* mMesh = nullptr;
		List<Asset<IMaterial>*> mMaterialList;
		SharedPtr<IMaterial>    mNullMaterial;


		SharedPtr<ScheduleHandle> mPushRenderSceneObjectScheduleHandle;
	public:
		virtual ~StaticMeshRenderer() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
	public:
		void SetMesh(const String& path);
		void SetMaterial(const String& path);
		void SetMaterial(i32 slot, const String& path);
		void AddMaterial(const String& path);
		i32 GetMaterialCount() const;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual void OnChange(const ChangeData & data) override;
		virtual void OnInspectorGUI() override;
	private:
		void OnInspector_MeshGUI();
		void OnInspector_MaterialGUI();
		EScheduleResult PushRenderSceneObject();
	};
}