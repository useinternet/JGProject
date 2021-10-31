#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Graphics/JGGraphics.h"
#include "Class/Game/GameWorld.h"
#include "Common/DragAndDrop.h"
namespace JG
{
	SpriteRenderer::SpriteRenderer()
	{
		mSpriteRI = CreateUniquePtr<Standard2DRenderItem>();
		mSpriteRI->WorldMatrix = JMatrix::Identity();
		mSpriteRI->Color = Color::White();
	}
	void SpriteRenderer::Awake()
	{
		mSpriteRI->TargetLayer = GetOwner()->GetLayer();
	}
	void SpriteRenderer::Start()
	{
		BaseRenderer::Start();
		if (mSprite == nullptr)
		{
			SetSprite("Asset/Resources/NullTexture.jgasset");
		}

	}
	void SpriteRenderer::Destory()
	{
		BaseRenderer::Destory();
	}
	void SpriteRenderer::Update()
	{

	}

	void SpriteRenderer::LateUpdate()
	{

	}

	void SpriteRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		BaseRenderer::MakeJson(jsonData);
		jsonData->AddMember("Color", JVector4(mSpriteRI->Color));




		if (mSprite)
		{
			jsonData->AddMember("SpritePath", mSprite->GetAssetPath());
		}
	

	}

	void SpriteRenderer::LoadJson(SharedPtr<JsonData> jsonData)
	{
		BaseRenderer::LoadJson(jsonData);
		auto val = jsonData->GetMember("Color");
		if (val)
		{
			mSpriteRI->Color = Color(val->GetVector4());
		}
		val = jsonData->GetMember("SpritePath");
		if (val && val->IsString())
		{
			SetSprite(val->GetString());
		}
	}

	void SpriteRenderer::SetSprite(const String& path)
	{
		auto assetManager = GetGameWorld()->GetAssetManager();
		if (assetManager->GetAssetFormat(path) != EAssetFormat::Texture)
		{
			return;
		}
		mSprite = assetManager->RequestOriginAsset<ITexture>(path);
	}


	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

	}
	void SpriteRenderer::OnInspectorGUI()
	{
		BaseRenderer::OnInspectorGUI();
		ImGui::AlignTextToFramePadding();
		f32 label_width = ImGui::CalcTextSize("Sprite").x;

		ImGui::Color4_OnGUI("Color", mSpriteRI->Color, label_width);
		if (mSprite)
		{
			ImGui::AssetField_OnGUI("Sprite ", mSprite->GetAssetName(), EAssetFormat::Texture, [&](const String& path)
			{
				mSprite = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>(path);
			}, label_width);

		}
		else
		{
			ImGui::AssetField_OnGUI("Sprite ", "None", EAssetFormat::Texture, [&](const String& path)
			{
				mSprite = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>(path);
			}, label_width);
		}







		//ImGui::Text("Color "); ImGui::SameLine();
		//ImGui::ColorEdit4("##Color Editor", (float*)(&mSpriteRI->Color));
		
		//String path;
		//if (mSprite)
		//{
		//	if (ImGui::AssetField("Sprite ", mSprite->GetAssetName(), EAssetFormat::Texture, path) == true)
		//	{
		//		mSprite = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>(path);
		//	}
		//}
		//else
		//{
		//	if (ImGui::AssetField("Sprite ", "None", EAssetFormat::Texture, path) == true)
		//	{
		//		mSprite = GetGameWorld()->GetAssetManager()->RequestOriginAsset<ITexture>(path);
		//	}
		//}
	}

	void SpriteRenderer::PushRenderSceneObject()
	{
		if (mSprite == nullptr) return;

		auto sceneObject = CreateSharedPtr<Graphics::PaperObject>();
		auto transform   = GetOwner()->GetTransform();
		sceneObject->WorldMatrix = transform->GetWorldMatrix();

		if (mSprite && mSprite->Get()->IsValid())
		{
			auto info = mSprite->Get()->GetTextureInfo();
			sceneObject->Texture = mSprite->Get();
			f32 adjust = (f32)info.PixelPerUnit / (f32)GameSettings::GetUnitSize();
			f32 spriteWidth = info.Width * adjust;
			f32 spriteHeight = info.Height * adjust;

			if (mSpriteSize.x != spriteWidth || mSpriteSize.y != spriteHeight)
			{
				JBBox bbox;
				bbox.min = JVector3(-spriteWidth * 0.5f, -spriteHeight * 0.5f, 0.0f);
				bbox.max = JVector3(spriteWidth * 0.5f, spriteHeight * 0.5f, 0.0f);
				GetOwner()->SetPickingBoundingBox(bbox);
				mSpriteSize = JVector2(spriteWidth, spriteHeight);

			}
			sceneObject->WorldMatrix = JMatrix::Scaling(JVector3(spriteWidth, spriteHeight, 1)) * sceneObject->WorldMatrix;
		}


		GetGameWorld()->PushRenderSceneObject(sceneObject);

	}

}
