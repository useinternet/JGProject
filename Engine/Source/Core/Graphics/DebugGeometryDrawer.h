#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"
#include "Class/Asset/Asset.h"






namespace JG
{
	class ISubMesh;
	class IMesh;
	class IVertexBuffer;
	class IIndexBuffer;
	class DebugGeometryDrawer : public GlobalSingleton<DebugGeometryDrawer>
	{
		friend Application;
	private:
		class DebugLineData : public IJGObject
		{
			JGCLASS
		public:
			JRay  ray;
			f32   length;
			Color color;
		public:
			DebugLineData(const JRay& ray, f32 len, const Color& color) : ray(ray), length(len), color(color) {}
		};
		class DebugLinesData : public IJGObject
		{
			JGCLASS
		public:
			List<JVector3> points;
			Color color;
		public:
			DebugLinesData(const List<JVector3>& points, const Color& color) : points(points), color(color) {}
		};
		class DebugBoxData : public IJGObject
		{
			JGCLASS
		public:
			JVector3 location;
			JQuaternion quaternion;
			JVector3 size;
			Color color;
		public:
			DebugBoxData(const JVector3& location, const JQuaternion& quat, const JVector3& Size, const Color& color) :
				location(location), quaternion(quat), size(Size), color(color) {}
		};
		class DebugSphereData : public IJGObject
		{
			JGCLASS
		public:
			JVector3 center;
			f32 r;
			Color color;
		public:
			DebugSphereData(const JVector3& center, f32 r, const Color& color) : center(center), r(r), color(color) {}
		};


		struct DebugCBData
		{
			Color   Color;
			JMatrix WorldMatrix;
		};
	private:
		const f32 mThick = 0.05f;
		SharedPtr<ISubMesh>      mSubMesh;
		SharedPtr<IMesh>         mMesh;
		SharedPtr<IMaterial>     mDebugMaterial;
		List<DebugCBData>        mDebugDataList;
		u32 mLineCount = 0;


		std::mutex      mMutex;
		std::atomic_int mTaskCount = 0;
	public:
		DebugGeometryDrawer();
		virtual ~DebugGeometryDrawer();
	public:
		void DrawDebugLine(const JRay& ray, f32 length, const Color& color);
		void DrawDebugLines(const List<JVector3>& points, const Color& color);


		void DrawDebugBox(const JVector3& location, const JQuaternion& quat, const JVector3& Size, const Color& color);
		void DrawDebugSphere(const JVector3& location, f32 r, const Color& color);

	private:
		void AddDebugLines(const List<JVector3>& points, const Color& color);
		void AddDebugLine(const JRay& ray, f32 length, const Color& color);
		void CreateDebugLine(f32 thick, f32 length, List<JGDebugVertex>& out_vertices, List<u32>& out_indices);
	};
}
