#include "pch.h"
#include "EditorUIRenderer.h"
#include "Graphics/JGGraphics.h"



namespace JG
{
	EditorUIRenderer::EditorUIRenderer()
	{
		Init();
	}
	bool EditorUIRenderer::Begin(SharedPtr<ITexture> targetTexture)
	{
		if (targetTexture == nullptr || targetTexture->IsValid() == false)
		{
			return false;
		}
		IGraphicsAPI* api = JGGraphics::GetInstance().GetGraphicsAPI();
		TextureInfo texInfo = targetTexture->GetTextureInfo();


		SharedPtr<IGraphicsContext> context = api->GetGraphicsContext(GRAPHICS_COMMAND_QUEUE_ID, GRAPHICS_CONTEXT_ID);
		if (mIsClearWhiteTexture == false)
		{
			auto api = JGGraphics::GetInstance().GetGraphicsAPI();
			JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
			context->ClearRenderTarget({ mWhiteTexture }, nullptr);
			mIsClearWhiteTexture = true;
		}



		context->SetViewports({ Viewport(texInfo.Width, texInfo.Height) });
		context->SetScissorRects({ ScissorRect(0,0, texInfo.Width,texInfo.Height) });
		context->SetRenderTarget({ targetTexture }, nullptr);

		StartBatch();
		return true;
	}
	void EditorUIRenderer::End()
	{
		NextBatch();
	}
	void EditorUIRenderer::DrawCall(const JMatrix& transform, SharedPtr<ITexture> texture, const Color& color)
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
	void EditorUIRenderer::Init()
	{
		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
		inputLayout->Add(EShaderDataType::_float4, "COLOR", 0);
		inputLayout->Add(EShaderDataType::_int, "TEXTUREINDEX", 0);

		mEditorUIShader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardEditorUIShader);

		TextureInfo textureInfo;
		textureInfo.Width = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
		mWhiteTexture = ITexture::Create("WhiteTexture", textureInfo);

		mFrameResource.QuadMesh = IMesh::Create("Renderer2D_QuadMesh");
		mFrameResource.QuadMesh->SetInputLayout(inputLayout);
		mFrameResource.QuadMesh->AddMesh(ISubMesh::Create("Renderer2D_QuadSubMesh"));
		mFrameResource.QuadVBuffer = IVertexBuffer::Create("Renderer2D_VBuffer", EBufferLoadMethod::CPULoad);
		mFrameResource.QuadIBuffer = IIndexBuffer::Create("Renderer2D_IBuffer", EBufferLoadMethod::CPULoad);
		mFrameResource.QuadMesh->GetSubMesh(0)->SetVertexBuffer(mFrameResource.QuadVBuffer);
		mFrameResource.QuadMesh->GetSubMesh(0)->SetIndexBuffer(mFrameResource.QuadIBuffer);
	
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

		StartBatch();
	}
	void EditorUIRenderer::StartBatch()
	{
		mQuadCount = 0;
		mTextureCount = 1;
	}
	void EditorUIRenderer::NextBatch()
	{
		if (mQuadCount == 0) return;
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");


		SharedPtr<IGraphicsContext> context = api->GetGraphicsContext(GRAPHICS_COMMAND_QUEUE_ID, GRAPHICS_CONTEXT_ID);
	

		u32 quadVertexCount = mQuadCount * QuadVertexCount;
		u32 quadIndexCount = mQuadCount * QuadIndexCount;
		mFrameResource.QuadVBuffer->SetData(mVertices.data(), sizeof(QuadVertex), quadVertexCount);
		mFrameResource.QuadIBuffer->SetData(mIndices.data(), quadIndexCount);


		context->SetInputLayout(mFrameResource.QuadMesh->GetInputLayout());
		context->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);


		context->BindShader(mEditorUIShader);
		context->BindTextures((u32)Renderer::ERootParam::Texture2D, mTextureArray);
		context->BindVertexAndIndexBuffer(mFrameResource.QuadVBuffer, mFrameResource.QuadIBuffer);
		context->DrawIndexed(quadIndexCount);

		StartBatch();
	}
}

