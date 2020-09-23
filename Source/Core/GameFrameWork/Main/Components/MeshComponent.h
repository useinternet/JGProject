#pragma once
#include "PrimitiveComponent.h"


namespace GFW
{

	class MeshComponent : public PrimitiveComponent
	{


	public:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		bool IsVisible() const {
			return m_IsVisible;
		}
		void SetVisible(bool visible) {
			m_IsVisible = visible;
		}
	private:

		bool m_IsVisible = true;
	};



}
