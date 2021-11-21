#include "pch.h"
#include "SpriteRenderer.h"
#include "Transform.h"

#include "Graphics/JGGraphics.h"
#include "GameWorld.h"
#include "Common/DragAndDrop.h"
namespace JG
{
	void SpriteRenderer::Awake()
	{
		BaseRenderer::Awake();
		mPushRenderSceneObjectScheduleHandle = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_PushSceneObject, SCHEDULE_BIND_FN(&SpriteRenderer::PushRenderSceneObject));
	}
	void SpriteRenderer::Start()
	{
		BaseRenderer::Start();
	}
	void SpriteRenderer::Destory()
	{
		BaseRenderer::Destory();
		if (mPushRenderSceneObjectScheduleHandle != nullptr)
		{
			mPushRenderSceneObjectScheduleHandle->Reset();
			mPushRenderSceneObjectScheduleHandle = nullptr;
		}
	}
	void SpriteRenderer::Update()
	{
		BaseRenderer::Update();
	}

	void SpriteRenderer::LateUpdate()
	{
		BaseRenderer::LateUpdate();

	}

	void SpriteRenderer::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		BaseRenderer::MakeJson(jsonData);
		jsonData->AddMember("Color", JVector4(GetColor()));

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
			SetColor(Color(val->GetVector4()));
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

	void SpriteRenderer::SetColor(const Color& color)
	{
		mColor = color;
	}

	const Color& SpriteRenderer::GetColor() const
	{
		return mColor;
	}

	const Asset<ITexture>* SpriteRenderer::GetSprite() const
	{
		return mSprite;
	}


	void SpriteRenderer::OnChange(const ChangeData& data)
	{
		BaseRenderer::OnChange(data);

	}
	EScheduleResult SpriteRenderer::PushRenderSceneObject()
	{
		if (IsActive() == false)
		{
			return EScheduleResult::Continue;
		}
		auto sceneObject = CreateSharedPtr<Graphics::PaperObject>();
		auto transform   = GetOwner()->GetTransform();
		sceneObject->WorldMatrix = transform->GetWorldMatrix();

		if (mSprite && mSprite->Get()->IsValid())
		{
			sceneObject->Texture = mSprite->Get();
		}
		else
		{
			sceneObject->Texture = ITexture::NullTexture();
		}

		auto info = sceneObject->Texture->GetTextureInfo();
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
		GetGameWorld()->PushRenderSceneObject(sceneObject);
		return EScheduleResult::Continue;

	}

}