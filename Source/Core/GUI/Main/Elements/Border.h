#pragma once

#include "Element.h"



namespace GUI
{
	class GUI_API Border : public Element
	{
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Tick()  override;
		virtual void Destroy()  override;
	public:
		void SetDrawPriority(uint64_t priority);
		void SetAutoPriority(bool is_auto);
		void SetColor(const JColor& color);
		void SetThickness(float thick);
		


		const JColor& GetColor() const;
		float GetThickness() const {
			return m_Thickness;
		}
	public:
		void UpdateMesh();
	private:
		class ShapeComponent* m_ShapeCom = nullptr;
		float m_Thickness = 1.0f;
	};
}