#pragma once
#include "InspectorUI.h"
#include "GameNode.h"
#include "GameWorld.h"
#include "GameComponent.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/SkyDome.h"
#include "Components/SpriteRenderer.h"
#include "Components/StaticMeshRenderer.h"



namespace JG
{

	class GameNodeInspectorUI			: public InspectorUI<GameNode>			{	virtual void OnGUI_Impl(GameNode*			obj) override;};
	class GameWorldInspectorUI			: public InspectorUI<GameWorld>			{	virtual void OnGUI_Impl(GameWorld*			obj) override;};
	class TransformInspectorUI			: public InspectorUI<Transform>			{	virtual void OnGUI_Impl(Transform*			obj) override;};
	class CameraInspectorUI				: public InspectorUI<Camera>				{	virtual void OnGUI_Impl(Camera*				obj) override;};
	class EditorCameraInspectorUI : public InspectorUI<EditorCamera> { virtual void OnGUI_Impl(EditorCamera* obj) override; };
	class PointLightInspectorUI			: public InspectorUI<PointLight>			{	virtual void OnGUI_Impl(PointLight*			obj) override;};
	class DirectionalLightInspectorUI : public InspectorUI<DirectionalLight> { virtual void OnGUI_Impl(DirectionalLight* obj) override; };
	class SkyDomeInspectorUI			: public InspectorUI<SkyDome>				{	virtual void OnGUI_Impl(SkyDome*			obj) override;};
	class SpriteRendererInspectorUI		: public InspectorUI<SpriteRenderer>		{	virtual void OnGUI_Impl(SpriteRenderer*		obj) override;};
	class StaticMeshRendererInspectorUI : public InspectorUI<StaticMeshRenderer>	{	virtual void OnGUI_Impl(StaticMeshRenderer* obj) override;};
}