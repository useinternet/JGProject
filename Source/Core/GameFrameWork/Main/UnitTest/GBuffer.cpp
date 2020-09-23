#include "pch.h"
#include "GBuffer.h"

using namespace std;
namespace GFW
{
//	void GBuffer::Start()
//	{
//		GraphicsIF::RegisterInVaildTexture(TT("../Source/Resource/Texture/Engine/InvaildTexture.png"));
//		//LoadMeshs(TT("../Source/Resource/Test/Model/"));
//		//LoadTextures(TT("../Source/Resource/Test/Textures/"));
//		LoadMaterial();
//
//
//
//
//		m_Scene       = GraphicsIF::RequestScene(GE::RendererMode::_3D);
//		m_Sun         = GraphicsIF::RequestDirectionalLight();
//		m_Cam.SetLens(JG_PI * 0.25f, 1920, 1080, 10000000.0f, 0.1f, false);
//		m_Scene->BindCamera(&m_Cam);
//		m_Cam.SetPosition({ 0, 300, -400.0f });
//
//
//		m_Sun->GetSDDataRef().diffuse = JColor(0.8f, 0.8f, 0.9f, 1.0f);
//		m_Sun->GetSDDataRef().dir = { 0.3f,-1.0f,1.0f };
//
//
//
//	/*	{
//			ObjectData data;
//			data.name = TT("Floor0");
//			data.meshFile = TT("Floor_400x400");
//			data.material = TT("Floor");
//			data.rotation = { 90.0f, 0.0f, 0.0f };
//			data.position = { 0.0f ,0.0f, 0.0f };
//			CreateObject(data);
//
//			data.name = TT("Floor1");
//			data.position = { -400.0f,0.0f, 0.0f };
//			CreateObject(data);
//
//			data.name = TT("Floor2");
//			data.position = { 0.0f,0.0f, 400.0f };
//			CreateObject(data);
//
//			data.name = TT("Floor3");
//			data.position = { -400.0f,0.0f, 400.0f };
//			CreateObject(data);
//		}
//		{
//			ObjectData data;
//			data.name = TT("Sphere");
//			data.meshFile = TT("Shape_Sphere");
//			data.material = TT("Default");
//			data.rotation = { 90.0f, 0.0f, 0.0f };
//			data.position = { 0.0f ,0, 0.0f };
//			CreateObject(data);
//		}
//		{
//			ObjectData data;
//			data.name = TT("Man");
//			data.meshFile = TT("test");
//			data.material = TT("NoTexture");
//			data.position = { 250.0f ,0, 0.0f };
//			CreateObject(data);
//		}
//		{
//			ObjectData data;
//			data.name = TT("WideCapsule");
//			data.meshFile = TT("Shape_WideCapsule");
//			data.material = TT("NoTexture");
//			data.rotation = { 90.0f, 0.0f, 0.0f };
//			data.position = { -250.0f ,0, 0.0f };
//			CreateObject(data);
//		}*/
//
//
//
//		std::ofstream fout;
//		fout.open("InstanceTempData", std::ios::binary);
//		fout << (size_t)(m_Sun.Get()) << std::endl;
//		fout << (size_t)(m_Scene->GetBindedCamera()) << std::endl;
//
//		fout << m_Objects.size() << endl;
//		for (auto& o : m_Objects)
//		{
//			fout << (size_t)o.second->ptr.Get() << endl;
//		}
//
//		fout.close();
//
//
//
//		auto obj = GetSceneObject(L"Man");
//		obj->ptr->GetMaterial()->BindTexture("Normal", GetTexture(TT("T_Water_N")));
//
//		for (int i = 0; i < 4; ++i)
//		{
//			obj = GetSceneObject(L"Floor" + to_wstring(i));
//			obj->ptr->GetMaterial()->BindTexture("Normal", GetTexture(L"T_Water_N"));
//			obj->ptr->GetMaterial()->SetVarAsFloat4("Color", JColor(0.3f, 0.45f, 0.9f, 1.0f));
//		}
//	
//	}
//
//	void GBuffer::Tick(float tick)
//	{
//
//
//
//		for (auto& obj : m_Objects)
//		{
//			m_Scene->PushObject(obj.second->ptr.Get());
//		}
//		m_Scene->PushLight(m_Sun.Get());
//		GraphicsIF::DrawCall(m_Scene);
//
//
//
//
//	}
//
//	void GBuffer::Destroy()
//	{
//
//	}
//
//	uint64_t GBuffer::GetID()
//	{
//		return (uint64_t)m_Scene->GetSceneTexture();
//	}
//	void GBuffer::LoadMeshs(const std::wstring& path)
//	{
//		fs::path p(path);
//		fs::directory_iterator dir_iter(p);
//		for (auto& iter = dir_iter; iter != fs::end(dir_iter); ++iter)
//		{
//			if (fs::is_directory((*iter))) continue;
//			auto filePath = (*iter).path();
//			MeshData data;
//			MeshGenerator::LoadFromFile(filePath.wstring(), data.v, data.i);
//			std::wstring fileName = filePath.filename();
//			std::wstring name = fileName.substr(0, fileName.length() - strlen(MESH_FILEFORMAT));
//			m_MeshDatas[name] = std::move(data);
//		}
//	}
//	void GBuffer::LoadTextures(const std::wstring& path)
//	{
//		fs::path p(path);
//		fs::directory_iterator dir_iter(p);
//		for (auto& iter = dir_iter; iter != fs::end(dir_iter); ++iter)
//		{
//			auto ref = GraphicsIF::LoadTexture((*iter).path().wstring());
//			std::wstring fileName = (*iter).path().filename().wstring();
//			fileName = fileName.substr(0, fileName.length() - 4);
//			m_Textures[fileName] = move(ref);
//			JGLOG(LogLevel::Info, GBuffer, "Texture Load : %s" , fileName);
//		}
//	
//	}
//	void GBuffer::LoadMaterial()
//	{
//
//		//
//		{
//			GE::MaterialProperty ppt;
//			ppt.rendererMode = GE::RendererMode::_3D;
//			ppt.blendMode = GE::BlendMode::Opaque;
//			MaterialRef m = GraphicsIF::RequestMaterial(L"NormalMapping", ppt);
//			m->DefineTexture("Albedo", nullptr);
//			m->DefineTexture("Normal", nullptr);
//			m->SetCode(R"(
//pout.Albedo     = Albedo_Texture.Sample(g_AnisotropicSampler, pin.tex);
//float3 normalT  = Normal_Texture.Sample(g_AnisotropicSampler, pin.tex).rgb;
//pout.Normal.xyz = BumpMapping(normalT, pin.normalW, pin.tangentW);
//)"
//);
//
//			GraphicsIF::PreCompileMaterial(m.Get());
//			m_Materials[L"NormalMapping"] = move(m);
//		}
//		{
//			GE::MaterialProperty ppt;
//			ppt.rendererMode = GE::RendererMode::_3D;
//			ppt.blendMode = GE::BlendMode::Opaque;
//			MaterialRef m = GraphicsIF::RequestMaterial(L"Floor", ppt);
//			m->DefineVarAsFloat4("Color", JColor::White());
//			m->DefineTexture("Normal", nullptr);
//			m->SetCode(R"(
//pout.Albedo     = Color_Data;
//float3 normalT  = Normal_Texture.Sample(g_AnisotropicSampler, pin.tex).rgb;
//pout.Normal.xyz = BumpMapping(normalT, pin.normalW, pin.tangentW);
//)"
//);
//
//			GraphicsIF::PreCompileMaterial(m.Get());
//			m_Materials[L"Floor"] = move(m);
//		}
//		{
//			GE::MaterialProperty ppt;
//			ppt.rendererMode = GE::RendererMode::_3D;
//			ppt.blendMode = GE::BlendMode::Opaque;
//			MaterialRef m = GraphicsIF::RequestMaterial(L"NoTexture", ppt);
//			m->DefineVarAsFloat4("Color", JColor::White());
//			m->SetCode(R"(
//pout.Albedo     = Color_Data;
//)"
//);
//			GraphicsIF::PreCompileMaterial(m.Get());
//			m_Materials[L"NoTexture"] = move(m);
//		}
//
//		{
//			GE::MaterialProperty ppt;
//			ppt.rendererMode = GE::RendererMode::_3D;
//			ppt.blendMode = GE::BlendMode::Opaque;
//			MaterialRef m = GraphicsIF::RequestMaterial(L"ManMat", ppt);
//			m->DefineTexture("Normal", nullptr);
//			m->SetCode(R"(
//pout.Albedo     = float4(0.95f,0.85f,1.0f,1.0f);
//float3 normalT  = Normal_Texture.Sample(g_AnisotropicSampler, pin.tex).rgb;
//pout.Normal.xyz = BumpMapping(normalT, pin.normalW, pin.tangentW);
//)"
//);
//			GraphicsIF::PreCompileMaterial(m.Get());
//			m_Materials[L"ManMat"] = move(m);
//	
//		}
//		{
//			GE::MaterialProperty ppt;
//			ppt.rendererMode = GE::RendererMode::_3D;
//			ppt.blendMode = GE::BlendMode::Opaque;
//			MaterialRef m = GraphicsIF::RequestMaterial(L"Default", ppt);
//			m->DefineTexture("Albedo", nullptr);
//			m->DefineTexture("Normal", nullptr);
//			m->SetCode(R"(
//pout.Albedo     = Albedo_Texture.Sample(g_AnisotropicSampler, pin.tex);
//)"
//);
//
//			GraphicsIF::PreCompileMaterial(m.Get());
//			m_Materials[L"Default"] = move(m);
//		}
//
//	}
//	void GBuffer::CreateObject(const ObjectData& data)
//	{
//		m_Objects[data.name] = make_shared<ObjectData>();
//		m_Objects[data.name]->ptr = GraphicsIF::RequestSceneObject();
//		auto obj = m_Objects[data.name]->ptr.Get();
//		auto meshData = GetMesh(data.meshFile);
//		auto material = GetMaterial(data.material);
//
//
//		obj->BindMesh(meshData->v.data(), meshData->v.size(), meshData->i.data(), meshData->i.size());
//		obj->BindMaterial(material);
//		
//
//
//
//		ConvertWorldMatrix(data.name, data.position, data.rotation);
//	}
//	void GBuffer::ConvertWorldMatrix(const std::wstring& name, const JVector3& pos, const JVector3& rot)
//	{
//		auto obj = GetSceneObject(name);
//
//		auto T = JMatrix::Translation(pos);
//		auto R = JMatrix::Rotation( JVector3
//			( DirectX::XMConvertToRadians(rot.x) ,
//			  DirectX::XMConvertToRadians(rot.y) ,
//			  DirectX::XMConvertToRadians(rot.z) ));
//		auto m = R * T;
//		obj->boundingBox.min = m.TransformPoint(obj->boundingBox.min);
//		obj->boundingBox.max = m.TransformPoint(obj->boundingBox.max);
//
//		
//		obj->sdData.world = JMatrix::Transpose(m);
//		obj->ptr->BindInstanceData(&obj->sdData, 1);
//		obj->ptr->SetName(name);
//		
//	}
}

