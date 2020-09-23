#pragma once
#include "Components/NativeScriptComponent.h"




namespace JGUI
{
	class GUI_API Border : public NativeScriptComponent
	{

	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		void SetThickness(float thick);
		float GetThickness() const {
			return m_Thickness;
		}
	private:
		void UpdateMesh();
	private:
		class ShapeComponent* m_ShapeCom = nullptr;
		float m_Thickness = 1.0f;

	};
}