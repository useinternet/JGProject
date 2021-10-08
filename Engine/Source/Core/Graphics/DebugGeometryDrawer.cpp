#include "pch.h"
#include "DebugGeometryDrawer.h"
#include "Application.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Resource.h"
namespace JG
{
	DebugGeometryDrawer::DebugGeometryDrawer()
	{
		auto vBuffer = IVertexBuffer::Create("Debug_VBuffer", EBufferLoadMethod::CPULoad);
		auto iBuffer = IIndexBuffer::Create("Debug_IBuffer", EBufferLoadMethod::CPULoad);
		mSubMesh = ISubMesh::Create("Debug_SubMesh");
		mMesh    = IMesh::Create("Debug_Mesh");

		mMesh->AddMesh(mSubMesh);
		mMesh->SetInputLayout(JGDebugVertex::GetInputLayout());
		mSubMesh->SetVertexBuffer(vBuffer);
		mSubMesh->SetIndexBuffer(iBuffer);


		List<JGDebugVertex> vertices;
		List<u32>           indices;
		CreateDebugLine(mThick, 1.0f, vertices, indices);



		vBuffer->SetData(vertices.data(), sizeof(JGDebugVertex), vertices.size());
		iBuffer->SetData(indices.data(), indices.size());

		Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::EndSystem + 1000,
			[&]() -> EScheduleResult
		{
			if (mDebugMaterial == nullptr)
			{
				auto shader = ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::DebugShader);
				mDebugMaterial = IMaterial::Create("DebugMaterial", shader);
			}

			while (mTaskCount != 0) {}

			mSubMesh->SetInstanceCount(mLineCount);
			mDebugMaterial->SetStructDataArray("gDebugDataInstances", mDebugDataList.data(), mDebugDataList.size(), sizeof(DebugCBData));




			auto ri = CreateSharedPtr<DebugRenderItem>();
			ri->WorldMatrix = JMatrix::Identity();
			ri->Material = mDebugMaterial;
			ri->Mesh = mMesh;
		
			RequestPushRenderItemEvent e;
			e.RenderItem = ri;
			Application::GetInstance().SendEvent(e);

			mLineCount = 0;
			mDebugDataList.clear();
			return EScheduleResult::Continue;
		});
	}
	DebugGeometryDrawer::~DebugGeometryDrawer()
	{


	}
	void DebugGeometryDrawer::DrawDebugLine(const JRay& ray, f32 length , const Color& color)
	{
		++mTaskCount;

		Scheduler::GetInstance().ScheduleAsync([&](SharedPtr<IJGObject> userData)
		{
			auto data = userData->As<DebugLineData>();

			AddDebugLine(data->ray, data->length, data->color);
			--mTaskCount;
		}, CreateSharedPtr<DebugLineData>(ray, length, color));
	}
	void DebugGeometryDrawer::DrawDebugLines(const List<JVector3>& points, const Color& color)
	{
		if (points.size() <= 1) return;
		++mTaskCount;

		Scheduler::GetInstance().ScheduleAsync([&](SharedPtr<IJGObject> userData)
		{
			auto data = userData->As<DebugLinesData>();
			AddDebugLines(data->points, data->color);
			--mTaskCount;
		}, CreateSharedPtr<DebugLinesData>(points, color));

	}
	void DebugGeometryDrawer::DrawDebugBox(const JVector3& location, const JQuaternion& quat, const JVector3& Size, const Color& color)
	{
		++mTaskCount;
		Scheduler::GetInstance().ScheduleAsync([&](SharedPtr<IJGObject> userData)
		{
			auto data = userData->As<DebugBoxData>();
			f32 hx = data->size.x * 0.5f;
			f32 hy = data->size.y * 0.5f;
			f32 hz = data->size.z * 0.5f;

			JMatrix t = JMatrix::Translation(data->location);
			JMatrix r = JMatrix::Rotation(data->quaternion);
			JMatrix m = r * t;

			JVector3 l_t_f = JVector3(-hx, hy, -hz);  l_t_f = m.TransformPoint(l_t_f);
			JVector3 r_t_f = JVector3(hx, hy, -hz);   r_t_f = m.TransformPoint(r_t_f);
			JVector3 l_b_f = JVector3(-hx, -hy, -hz); l_b_f = m.TransformPoint(l_b_f);
			JVector3 r_b_f = JVector3(hx, -hy, -hz);  r_b_f = m.TransformPoint(r_b_f);


			JVector3 l_t_b = JVector3(-hx, hy, hz);	 l_t_b = m.TransformPoint(l_t_b);
			JVector3 r_t_b = JVector3(hx, hy, hz);	 r_t_b = m.TransformPoint(r_t_b);
			JVector3 l_b_b = JVector3(-hx, -hy, hz); l_b_b = m.TransformPoint(l_b_b);
			JVector3 r_b_b = JVector3(hx, -hy, hz);	 r_b_b = m.TransformPoint(r_b_b);


			List<JVector3> frontpoints;
			frontpoints.push_back(l_t_f);
			frontpoints.push_back(r_t_f);
			frontpoints.push_back(r_b_f);
			frontpoints.push_back(l_b_f);
			frontpoints.push_back(l_t_f);
			AddDebugLines(frontpoints, data->color);
			
			List<JVector3> backpoints;
			backpoints.push_back(l_t_b);
			backpoints.push_back(r_t_b);
			backpoints.push_back(r_b_b);
			backpoints.push_back(l_b_b);
			backpoints.push_back(l_t_b);
			AddDebugLines(backpoints, data->color);



			AddDebugLines({ l_t_f, l_t_b }, data->color);
			AddDebugLines({ r_t_f, r_t_b }, data->color);
			AddDebugLines({ l_b_f, l_b_b }, data->color);
			AddDebugLines({ r_b_f, r_b_b }, data->color);




			--mTaskCount;
		}, CreateSharedPtr<DebugBoxData>(location, quat, Size, color));
	}
	void DebugGeometryDrawer::DrawDebugSphere(const JVector3& center, f32 r, const Color& color)
	{


		++mTaskCount;
		Scheduler::GetInstance().ScheduleAsync([&](SharedPtr<IJGObject> userData)
		{
			auto data = userData->As<DebugSphereData>();	
			u32 devideCnt = 200;
			List<JVector3> widthPoints;
			
			{
				f32 phi = JG_PI * 0.5F;
				f32 thetaStep = JG_2PI / devideCnt;

				for (u32 i = 0; i <= devideCnt; ++i)
				{
					f32 theta = i * thetaStep;
					JVector3 p;
					p.x = data->r * sinf(phi) * cosf(theta) + data->center.x;
					p.y = data->r * cosf(phi) + data->center.y;
					p.z = data->r * sinf(phi) * sinf(theta) + +data->center.z;

					widthPoints.push_back(p);
				}

			}
			widthPoints.push_back(widthPoints[0]);
			List<JVector3> heightPoints;
			{
				f32 theta = JG_PI * 0.5f;
				f32 phiStep = JG_2PI / devideCnt;

				for (u32 i = 0; i <= devideCnt; ++i)
				{
					f32 phi = i * phiStep;
					JVector3 p;
					p.x = data->r * sinf(phi) * cosf(theta) + data->center.x;
					p.y = data->r * cosf(phi) + data->center.y;
					p.z = data->r * sinf(phi) * sinf(theta) + +data->center.z;

					heightPoints.push_back(p);
				}
			}
			heightPoints.push_back(heightPoints[0]);


			AddDebugLines(widthPoints, data->color);
			AddDebugLines(heightPoints, data->color);

			--mTaskCount;

		}, CreateSharedPtr<DebugSphereData>(center, r, color));


	}

	void DebugGeometryDrawer::AddDebugLines(const List<JVector3>& points, const Color& color)
	{
		if (points.size() <= 1) return;
		u64 cnt = points.size();
		for (u64 i = 0; i < cnt - 1; ++i)
		{
			JRay ray;
			ray.origin = points[i];
			ray.dir = JVector3::Normalize(points[i + 1] - points[i]);
			f32 length = JVector3::Length(points[i + 1] - points[i]);
			AddDebugLine(ray, length, color);
		}
	}

	void DebugGeometryDrawer::AddDebugLine(const JRay& ray, f32 length, const Color& color)
	{
		auto layDir = JVector3::Normalize(ray.dir);
		f32  xAngle = atan2((f64)layDir.y, (f64)layDir.z);
		f32  yAngle = atan2((f64)layDir.x, (f64)layDir.z);
		f32  zAngle = atan2((f64)layDir.y, (f64)layDir.x);

		JVector3 front = JVector3(0.0f, 0.0f, 1.0f);
		JVector3 axis = JVector3::Cross(front, layDir);


		if (JVector3::LengthSqrd(axis) == 0)
		{
			axis = JVector3(0.0f, 1.0f, 0.0f);
		}

		f32 dot = JVector3::Dot(front, layDir);
		f32 ang = std::acosf(dot);
		auto q = JQuaternion::RotatationAxis(axis, ang);


		JMatrix  s = JMatrix::Scaling(JVector3(1.0f, 1.0f, length));
		JMatrix  t = JMatrix::Translation(ray.origin);
		JMatrix  r = JMatrix::Rotation(q);
		JMatrix  m = s * r * t;

		DebugCBData cbData;
		cbData.Color = color;
		cbData.WorldMatrix = JMatrix::Transpose(m);

		std::lock_guard lock(mMutex);
		++mLineCount;
		mDebugDataList.push_back(cbData);
	}

	void DebugGeometryDrawer::CreateDebugLine(f32 thick, f32 length, List<JGDebugVertex>& out_vertices, List<u32>& out_indices)
	{

		f32 hx = 0.5f * thick;
		f32 hy = 0.5f * thick;


		out_vertices.clear();
		out_vertices.resize(8);

		out_vertices[0].Position = JVector3(-hx, -hy, 0.0f);
		out_vertices[1].Position = JVector3(-hx, +hy, 0.0f);
		out_vertices[2].Position = JVector3(+hx, +hy, 0.0f);
		out_vertices[3].Position = JVector3(+hx, -hy, 0.0f);
		out_vertices[4].Position = JVector3(-hx, -hy, 1.0f);
		out_vertices[5].Position = JVector3(-hx, +hy, 1.0f);
		out_vertices[6].Position = JVector3(+hx, +hy, 1.0f);
		out_vertices[7].Position = JVector3(+hx, -hy, 1.0f);


		out_indices.clear();
		out_indices.resize(36);
		out_indices[0] = 0; out_indices[1] = 1; out_indices[2] = 2;
		out_indices[3] = 0; out_indices[4] = 2; out_indices[5] = 3;
		out_indices[6] = 6; out_indices[7] = 5; out_indices[8] = 4;
		out_indices[9] = 7; out_indices[10] = 6; out_indices[11] = 4;
		out_indices[12] = 3; out_indices[13] = 2; out_indices[14] = 6 ;
		out_indices[15] = 3; out_indices[16] = 6; out_indices[17] = 7 ;
		out_indices[18] = 4 ; out_indices[19] = 1; out_indices[20] = 0;
		out_indices[21] = 4 ; out_indices[22] = 5; out_indices[23] = 1;	 
		out_indices[24] = 1; out_indices[25] = 5 ; out_indices[26] = 2;
		out_indices[27] = 5; out_indices[28] = 6; out_indices[29] = 2; 
		out_indices[30] = 7 ; out_indices[31] = 4; out_indices[32] = 0;
		out_indices[33] = 3; out_indices[34] = 7; out_indices[35] = 0;

	}

}