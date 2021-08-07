#include "pch.h"
#include "Renderer.h"
#include "Application.h"

#include "Class/Game/Components/Camera.h"
#include "Class/Asset/Asset.h"
#include "Graphics/Resource.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "GraphicsAPI.h"

namespace JG
{
	bool IRenderer::BeginBatch(const RenderInfo& info, List<SharedPtr<IRenderBatch>> batchList)
	{
		bool result = true;
		mBatchList = batchList;
		if (mBatchList.empty() == false)
		{
			for (auto& batch : mBatchList)
			{
				batch->SetCommandID(GetCommandID());
				batch->ConnectRenderer(this);
				if (batch->Begin(info) == false)
				{
					result = false;
				}
			}
		}
		return result;
	}

	void IRenderer::EndBatch()
	{
		for (auto& batch : mBatchList)
		{
			batch->End();
		}
		mBatchList.clear();
	}


	bool FowardRenderer::Begin(const RenderInfo& info, List<SharedPtr<IRenderBatch>> batchList)
	{
		if (mIsRun == true)
		{
			return false;
		}
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		mCurrentRenderInfo = info;
		if (info.TargetTexture == nullptr)
		{
			return false;
		}
		if (mRenderTarges.empty())
		{
			mRenderTarges.resize(MAX_RENDERTARGET, nullptr);
		}

		mRenderTarges[0] = info.TargetTexture;

		api->SetViewports(GetCommandID(), { Viewport(info.Resolutoin.x, info.Resolutoin.y) });
		api->SetScissorRects(GetCommandID() ,{ ScissorRect(0,0, info.Resolutoin.x,info.Resolutoin.y) });
		api->ClearRenderTarget(GetCommandID(),mRenderTarges, info.TargetDepthTexture);
		api->SetRenderTarget(GetCommandID(),mRenderTarges, info.TargetDepthTexture);
		
		return BeginBatch(info, batchList);
	}
	void FowardRenderer::DrawCall(const JMatrix& worldMatrix, SharedPtr<IMesh> mesh, List<SharedPtr<IMaterial>> materialList)
	{
		if (mesh == nullptr || materialList.empty())
		{
			return;
		}
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		mesh->SetCommandID(GetCommandID());
		if (mesh->Bind() == false)
		{
			JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetName());
		}





		auto transposedViewProj = JMatrix::Transpose(mCurrentRenderInfo.ViewProj);
		for (u64 i = 0; i < mesh->GetSubMeshCount(); ++i)
		{
			mesh->GetSubMesh(i)->SetCommandID(GetCommandID());
			if (mesh->GetSubMesh(i)->Bind() == false)
			{
				JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetSubMesh(0)->GetName());
				continue;
			}
			SharedPtr<IMaterial> material = nullptr;
			if (materialList.size() <= i)
			{
				material = materialList[0];
			}
			else
			{
				material = materialList[i];
			}
			auto transposedWorld = JMatrix::Transpose(worldMatrix);
			if (material->SetFloat4x4(ShaderScript::Standard3D::ViewProj, transposedViewProj) == false)
			{
				JG_CORE_INFO("{0} : Fail SetViewProjMatrix in ObjectParams", material->GetName());
				continue;
			}
			if (material->SetFloat4x4(ShaderScript::Standard3D::World, transposedWorld) == false)
			{
				JG_CORE_INFO("{0} : Fail SetWorldMatrix in ObjectParams", material->GetName());
				continue;
			}
			material->SetCommandID(GetCommandID());
			if (material->Bind() == false)
			{
				JG_CORE_INFO("{0} : Fail Material Bind", material->GetName());
				continue;
			}
			api->DrawIndexed(GetCommandID(), mesh->GetSubMesh(i)->GetIndexCount());
		}
		
		

	}
	void FowardRenderer::End()
	{
		EndBatch();
	}

	Render2DBatch::Render2DBatch()
	{
		auto bufferCnt = Application::GetInstance().GetGraphicsAPI()->GetBufferCount();
		mFrameResources.resize(bufferCnt);

		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
		inputLayout->Add(EShaderDataType::_float4, "COLOR", 0);
		inputLayout->Add(EShaderDataType::_int, "TEXTUREINDEX", 0);

		auto _2dShader = ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::Standard2DShader);

		TextureInfo textureInfo;
		textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mWhiteTexture = ITexture::Create("WhiteTexture", textureInfo);

		for (i32 i = 0; i < bufferCnt; ++i)
		{
			FrameResource rsc;
			rsc.QuadMesh = IMesh::Create("Renderer2D_QuadMesh");
			rsc.QuadMesh->SetInputLayout(inputLayout);
			rsc.QuadMesh->AddMesh(ISubMesh::Create("Renderer2D_QuadSubMesh"));



			rsc.QuadVBuffer = IVertexBuffer::Create("Renderer2D_VBuffer", EBufferLoadMethod::CPULoad);
			rsc.QuadIBuffer = IIndexBuffer::Create("Renderer2D_IBuffer", EBufferLoadMethod::CPULoad);

			rsc.QuadMesh->GetSubMesh(0)->SetVertexBuffer(rsc.QuadVBuffer);
			rsc.QuadMesh->GetSubMesh(0)->SetIndexBuffer(rsc.QuadIBuffer);

			rsc.Standard2DMaterial = IMaterial::Create("Standard2DMaterial", _2dShader);

			if (rsc.Standard2DMaterial->SetTexture("gTexture", 0, mWhiteTexture) == false)
			{
				JG_CORE_ERROR("Failed Set Texture in WhiteTexture");
				return;
			}
			rsc.Standard2DMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
			rsc.Standard2DMaterial->SetBlendState(0, EBlendStateTemplate::Transparent_Default);

			mFrameResources[i] = rsc;
		}

		mStandardQuadPosition[0] = JVector3(-0.5f, -0.5f, 0.0f);
		mStandardQuadPosition[1] = JVector3(-0.5f, +0.5f, 0.0f);
		mStandardQuadPosition[2] = JVector3(+0.5f, +0.5f, 0.0f);
		mStandardQuadPosition[3] = JVector3(+0.5f, -0.5f, 0.0f);
		mStandardQuadTexcoord[0] = JVector2(0.0f, 1.0f);
		mStandardQuadTexcoord[1] = JVector2(0.0f, 0.0f);
		mStandardQuadTexcoord[2] = JVector2(1.0f, 0.0f);
		mStandardQuadTexcoord[3] = JVector2(1.0f, 1.0f);

		mVertices.resize(MaxVertexCount, QuadVertex());
		mIndices.resize(MaxIndexCount, 0);

		u32 offset = 0;
		for (u32 i = 0; i < MaxIndexCount;)
		{
			mIndices[i++] = offset + 0;
			mIndices[i++] = offset + 1;
			mIndices[i++] = offset + 2;
			mIndices[i++] = offset + 0;
			mIndices[i++] = offset + 2;
			mIndices[i++] = offset + 3;
			offset += QuadVertexCount;
		}
		mTextureArray.resize(MaxTextureCount, nullptr);
		mTextureArray[0] = mWhiteTexture;

		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
		api->ClearRenderTarget(MAIN_GRAPHICS_COMMAND_ID, { mWhiteTexture }, nullptr);

		StartBatch();
	}

	Render2DBatch::~Render2DBatch()
	{
	
	}

	bool Render2DBatch::Begin(const RenderInfo& info)
	{
		if (GetConnectedRenderer() == nullptr)
		{
			return false;
		}
		mCurrFrameResource = &mFrameResources[info.CurrentBufferIndex];
		auto transposedViewProj = JMatrix::Transpose(info.ViewProj);
		if (mCurrFrameResource->Standard2DMaterial->SetFloat4x4(ShaderScript::Standard2D::ViewProj, transposedViewProj) == false)
		{
			JG_CORE_ERROR("Failed Set ViewProjMatrix in Renderer2D");
			return false;
		}
		return true;
	}

	void Render2DBatch::End()
	{
		NextBatch();
	}


	void Render2DBatch::DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color)
	{
		if (mQuadCount >= MaxQuadCount || mTextureCount >= MaxTextureCount)
		{
			NextBatch();
		}
		i32 textureIndex = 0;
		if (texture != nullptr && texture->IsValid() == true)
		{
			for (auto& tex : mTextureArray)
			{
				if (tex == nullptr || tex->GetTextureID() == texture->GetTextureID())
				{
					mTextureCount++;
					mTextureArray[textureIndex] = texture;
					if (mCurrFrameResource->Standard2DMaterial->SetTexture(ShaderScript::Standard2D::Texture, textureIndex, mTextureArray[textureIndex]) == false)
					{
						JG_CORE_ERROR("Failed Set Texture Slot : {0}", textureIndex);
					}
					break;
				}
				++textureIndex;
			}
		}

		u32 offset = mQuadCount * QuadVertexCount;
		for (u32 i = 0; i < QuadVertexCount; ++i)
		{
			u32 index = offset + i;
			mVertices[index].Pos = transform.TransformPoint(mStandardQuadPosition[i]);
			mVertices[index].Tex = mStandardQuadTexcoord[i];
			mVertices[index]._Color = color;
			mVertices[index].TextureIndex = textureIndex;
		}
		mQuadCount++;
	}
	void Render2DBatch::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture, const Color& color)
	{
		JMatrix transform =
		JMatrix::Scaling(JVector3(Size, 1.0f)) * JMatrix::Rotation(JVector3(0.0f, 0.0f, Math::ConvertToRadians(rotation))) * JMatrix::Translation(JVector3(Pos, 0.0f));
		DrawCall(transform, texture, color);
	}
	void Render2DBatch::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, const Color& color)
	{
		DrawCall(Pos, Size, rotation, nullptr, color);
	}
	void Render2DBatch::DrawCall(const JVector2& Pos, const JVector2& Size, float rotation, SharedPtr<ITexture> texture)
	{
		DrawCall(Pos, Size, rotation, texture, Color::White());
	}
	void Render2DBatch::DrawCall(const JVector2& Pos, const JVector2& Size, const Color& color)
	{
		DrawCall(Pos, Size, 0, color);
	}
	void Render2DBatch::DrawCall(const JVector2& Pos, const JVector2& Size, SharedPtr<ITexture> texture)
	{
		DrawCall(Pos, Size, 0, texture);
	}


	void Render2DBatch::StartBatch()
	{
		mQuadCount = 0;
		mTextureCount = 1;
	}

	void Render2DBatch::NextBatch()
	{
		if (mQuadCount == 0) return;
		
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		if (mCurrFrameResource->Standard2DMaterial->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind StandardMaterial");
			StartBatch();
			return;
		}
		
		u32 quadVertexCount = mQuadCount * QuadVertexCount;
		u32 quadIndexCount = mQuadCount * QuadIndexCount;
		
		
		mCurrFrameResource->QuadVBuffer->SetData(mVertices.data(), sizeof(QuadVertex), quadVertexCount);
		mCurrFrameResource->QuadIBuffer->SetData(mIndices.data(), quadIndexCount);
		mCurrFrameResource->QuadMesh->SetCommandID(GetCommandID());
		if (mCurrFrameResource->QuadMesh->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			StartBatch();
			return;
		}
		mCurrFrameResource->QuadMesh->GetSubMesh(0)->SetCommandID(GetCommandID());
		if (mCurrFrameResource->QuadMesh->GetSubMesh(0)->Bind() == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			StartBatch();
			return;
		}

		api->DrawIndexed(GetCommandID(), quadIndexCount);
		StartBatch();
	}



}