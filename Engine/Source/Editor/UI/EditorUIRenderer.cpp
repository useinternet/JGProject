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
		auto commandID    = JGGraphics::GetInstance().RequestCommandID();
		IGraphicsAPI* api = JGGraphics::GetInstance().GetGraphicsAPI();
		i32 index = api->GetBufferIndex();
		TextureInfo texInfo = targetTexture->GetTextureInfo();


		if (mIsClearWhiteTexture == false)
		{
			auto api = JGGraphics::GetInstance().GetGraphicsAPI();
			JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
			api->ClearRenderTarget(commandID, { mWhiteTexture }, nullptr);
			mIsClearWhiteTexture = true;
		}



		mCurrFrameResource = &mFrameResources[index];




		api->SetViewports(commandID, { Viewport(texInfo.Width, texInfo.Height) });
		api->SetScissorRects(commandID, { ScissorRect(0,0, texInfo.Width,texInfo.Height) });
		api->SetRenderTarget(commandID, { targetTexture }, nullptr);

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
		auto bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		mFrameResources.resize(bufferCnt);

		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
		inputLayout->Add(EShaderDataType::_float4, "COLOR", 0);
		inputLayout->Add(EShaderDataType::_int, "TEXTUREINDEX", 0);

		auto _editorUiShader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::StandardEditorUIShader);

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
			mFrameResources[i] = rsc;
		} 

		mEditorUIMaterial = IMaterial::Create("EditorUIMaterial", _editorUiShader);
		mEditorUIMaterial->SetDepthStencilState(EDepthStencilStateTemplate::NoDepth);
	
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
		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		if (mEditorUIMaterial->Bind(commandID) == false)
		{
			JG_CORE_ERROR("Failed Bind StandardMaterial");
			StartBatch();
			return;
		}

		api->SetTextures(commandID, mTextureArray);



		u32 quadVertexCount = mQuadCount * QuadVertexCount;
		u32 quadIndexCount = mQuadCount * QuadIndexCount;


		mCurrFrameResource->QuadVBuffer->SetData(mVertices.data(), sizeof(QuadVertex), quadVertexCount);
		mCurrFrameResource->QuadIBuffer->SetData(mIndices.data(), quadIndexCount);
		if (mCurrFrameResource->QuadMesh->Bind(commandID) == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			StartBatch();
			return;
		}
		if (mCurrFrameResource->QuadMesh->GetSubMesh(0)->Bind(commandID) == false)
		{
			JG_CORE_ERROR("Failed Bind QuadMesh");
			StartBatch();
			return;
		}
		api->DrawIndexed(commandID, quadIndexCount);
		StartBatch();
	}
}
