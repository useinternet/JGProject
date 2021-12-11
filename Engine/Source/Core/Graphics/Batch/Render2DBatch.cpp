#include "pch.h"
#include "Render2DBatch.h"
#include "Graphics/JGGraphics.h"
namespace JG
{
	Render2DBatch::Render2DBatch()
	{
		auto bufferCnt = JGGraphics::GetInstance().GetBufferCount();
		mFrameResources.resize(bufferCnt);

		auto inputLayout = InputLayout::Create();
		inputLayout->Add(EShaderDataType::_float3, "POSITION", 0);
		inputLayout->Add(EShaderDataType::_float2, "TEXCOORD", 0);
		inputLayout->Add(EShaderDataType::_float4, "COLOR", 0);
		inputLayout->Add(EShaderDataType::_int, "TEXTUREINDEX", 0);

		auto _2dShader = ShaderLibrary::GetInstance().GetShader(ShaderDefine::Template::Standard2DShader);

		TextureInfo textureInfo;
		textureInfo.Width      = 1; textureInfo.Height = 1; 	textureInfo.MipLevel = 1; 	textureInfo.ArraySize = 1;
		textureInfo.ClearColor = Color::White();
		textureInfo.Format     = ETextureFormat::R8G8B8A8_Unorm; textureInfo.Flags = ETextureFlags::Allow_RenderTarget;
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
		auto commandID = JGGraphics::GetInstance().RequestCommandID();

		if (mIsClearWhiteTexture == false)
		{
			auto api = JGGraphics::GetInstance().GetGraphicsAPI();
			JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");
			api->ClearRenderTarget(commandID, { mWhiteTexture }, nullptr);
			mIsClearWhiteTexture = true;
		}



		mCurrFrameResource = &mFrameResources[info.CurrentBufferIndex];



		//struct PassCB
		//{
		//	JMatrix ViewProjMatrix;
		//};

		//PassCB passCB;
		//passCB.ViewProjMatrix = JMatrix::Transpose(info.ViewProjMatrix);

		//mCurrFrameResource->Standard2DMaterial->SetPassData(commandID, &passCB, sizeof(PassCB));
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
		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		if (mCurrFrameResource->Standard2DMaterial->Bind(commandID) == false)
		{
			JG_CORE_ERROR("Failed Bind StandardMaterial");
			StartBatch();
			return;
		}

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