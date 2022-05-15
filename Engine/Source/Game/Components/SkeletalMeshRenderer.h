#pragma once

#pragma once
#include "BaseRenderer.h"
namespace JG
{
	class AnimationController;
	class Skeletone;
	class SkeletalMeshRenderer : public BaseRenderer
	{
		JGCLASS
	private:
		Asset<IMesh>* mMesh = nullptr;
		Asset<Skeletone>* mSkeletone = nullptr;
		Asset<AnimationController>* mAnimatioin = nullptr;
		List<Asset<IMaterial>*>     mMaterialList;
	public:
		virtual ~SkeletalMeshRenderer() = default;
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destory() override;
	protected:
		virtual void Update() override;
		virtual void LateUpdate() override;
		virtual void FixedUpdate() override;
	public:
		void SetMesh(const String& path);
		void SetMaterial(const String& path);
		void SetMaterial(i32 slot, const String& path);
		void SetSkeletone(const String& path);
		void SetAnimation(const String& path);


		void AddMaterial(const String& path = String());
		void PopMaterial();
		i32 GetMaterialCount() const;


		Asset<IMesh>* GetMesh() const;
		Asset<IMaterial>* GetMaterial(i32 slot) const;
		Asset<Skeletone>* GetSkeletone() const;
		Asset<AnimationController>* GetAnimation() const;
		void ForEach(const std::function<void(Asset<IMaterial>*)>& action);
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData) const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		virtual void OnChange(const ChangeData& data) override;
	private:
		void PushRenderSceneObject();
	};
}