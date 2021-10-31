#include "Mesh.h"
#include "pch.h"
#include "Mesh.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "JGGraphics.h"
#include "Graphics/Resource.h"
#include "Class/Asset/Asset.h"
#include "Class/Asset/AssetImporter.h"


namespace JG
{
    SharedPtr<ISubMesh> ISubMesh::Create(const String& name)
    {
        auto api = JGGraphics::GetInstance().GetGraphicsAPI();
        JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


        return api->CreateSubMesh(name);
    }
    SharedPtr<IMesh> IMesh::Create(const String& name)
    {
        auto api = JGGraphics::GetInstance().GetGraphicsAPI();
        JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

        
        return api->CreateMesh(name);
    }
    void IMesh::SetMeshStock(const StaticMeshAssetStock& stock)
    {
        auto cnt = stock.SubMeshNames.size();
        SetInputLayout(JGVertex::GetInputLayout());
        for (u64 i = 0; i < cnt; ++i)
        {
            auto vBuffer = IVertexBuffer::Create(stock.SubMeshNames[i] + "_VBuffer", EBufferLoadMethod::CPULoad);
            auto iBuffer = IIndexBuffer::Create(stock.SubMeshNames[i] + "_IBuffer", EBufferLoadMethod::CPULoad);
            auto subMesh = ISubMesh::Create(stock.SubMeshNames[i]);

            vBuffer->SetData(stock.Vertices[i].data(), sizeof(JGVertex), stock.Vertices[i].size());
            iBuffer->SetData(stock.Indices[i].data(), stock.Indices[i].size());
            subMesh->SetVertexBuffer(vBuffer);
            subMesh->SetIndexBuffer(iBuffer);
            AddMesh(subMesh);
        }
        SetBoundingBox(stock.BoundingBox);
    }
    SharedPtr<IMesh> IMesh::Create(const StaticMeshAssetStock& stock)
    {
        SharedPtr<IMesh> mesh = IMesh::Create(stock.Name);
        mesh->SetMeshStock(stock);
        return mesh;
    }

}