#pragma once
#include "Element.h"


namespace GUI
{
	class Transform;
	class ShapeComponent;
	class GUI_API ResizeGrip : public Element
	{
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Tick()  override;
		virtual void Destroy()  override;
	protected:
		virtual void OnMouseButtonDown(KeyCode bt) override;
		virtual void OnMouseButtonUp(KeyCode bt) override;
		virtual void OnMouseEnter() override;
		virtual void OnMouseExit() override;
	public:
		void BindTransform(Transform* transform);
		void SetHightlightColor(const JColor& color);
		void SetNormalColor(const JColor& color);
		void SetMinSize(const JVector2& size) {
			m_MinSize;
		}
		const JColor& GetHightlightColor() const {
			return m_HightlightColor;
		}
		const JColor& GetNormalColor() const {
			return m_NormalColor;
		}
		const JVector2 GetMinSize() const {
			return m_MinSize;
		}
	private:
		ShapeComponent* m_ShapeComponent = nullptr;
		Transform* m_BindedTransform     = nullptr;
		bool       m_IsResizing          = false;
		bool       m_IsMouseIn           = false;
		JVector2   m_MousePosWhilePressed;
		JVector2   m_MinSize;
		
		JColor m_HightlightColor = JColor::White();
		JColor m_NormalColor     = JColor::White();

		float m_ResizeTick = 0.0f;
		float m_ResizeTimePerTick = 0.01f;
	};
}