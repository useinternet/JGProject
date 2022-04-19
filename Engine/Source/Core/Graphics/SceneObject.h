#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"


namespace JG
{
	class ITexture;
	class IMesh;
	class IMaterial;

	namespace Graphics
	{
		ENUM_FLAG(ESceneObjectFlags)
		enum class ESceneObjectFlags
		{
			None = 0,
			Ignore_RayTracing_Bottom_Level_AS = 0x00001,
			No_Shadow = 0x00002,
		};

		enum class ESceneObjectType
		{
			Static,
			Skeletal,
			Paper,
			Debug,
		};


		// SceneObject //
		class SceneObject
		{
		public:
			JMatrix	WorldMatrix = JMatrix::Identity();
			ESceneObjectFlags Flags = ESceneObjectFlags::None;
			u64     Layer = 0;

		public:
			virtual ~SceneObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const = 0;
			virtual bool IsValid() const = 0;
		};

		class PaperObject : public SceneObject
		{
		public:
			Color  Color = Color::White();
			SharedPtr<ITexture> Texture = nullptr;
		public:
			virtual ~PaperObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Paper; }
			virtual bool IsValid() const override { return true; }
		};


		class StaticRenderObject : public SceneObject
		{
		public:
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		public:
			virtual ~StaticRenderObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Static; }
			virtual bool IsValid() const override;
		};

		class SkeletalRenderObject : public SceneObject
		{

		public:
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		public:
			virtual ~SkeletalRenderObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Skeletal; }
			virtual bool IsValid() const override;
		};





		class DebugRenderObject : public SceneObject
		{
		public:
			SharedPtr<IMesh> Mesh;
			List<SharedPtr<IMaterial>> MaterialList;
		public:
			virtual ~DebugRenderObject() = default;
		public:
			virtual ESceneObjectType GetSceneObjectType() const override { return ESceneObjectType::Debug; }
			virtual bool IsValid() const override;
		};

	}
}