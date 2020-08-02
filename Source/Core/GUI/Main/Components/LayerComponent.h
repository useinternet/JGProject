#pragma once

#include "Component.h"

namespace GUI
{
	class GUI_API LayerComponent : public Component
	{
	protected:
		virtual void Awake() override;
		virtual void Tick() override;
		virtual void Destroy() override;

	public:
		void SetLayer(uint64_t layer) {
			m_Layer = layer;
		}
		uint64_t GetLayer() const {
			return m_Layer;
		}
		uint64_t IssueLayer() {
			return m_Layer + m_LayerOffset++;
		}
	public:
		uint64_t m_Layer = 0;
		uint64_t m_LayerOffset = 0;
	};
}