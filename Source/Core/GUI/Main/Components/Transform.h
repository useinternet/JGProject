#pragma once

#include "Component.h"


namespace GUI
{
	enum class AnchorPreset
	{
		// dot
		Left_Top,
		Center_Top,
		Right_Top,
		Left_Center,
		Center_Center,
		Right_Center,
		Left_Bottom,
		Center_Bottom,
		Right_Bottom,


		// Stretch
		Top_HStretch,
		Center_HStretch,
		Bottom_HStretch,
		Left_VStretch,
		Center_VStretch,
		Right_VStretch,
		Full_Stretch
	};
	class Element;
	class GUI_API Transform : public Component
	{
	public:
		void SetLocalLocation(const JVector2& pos);
		void SetLocalLocation(float x, float y) {
			SetLocalLocation(JVector2(x, y));
		}
		void SetLocation(const JVector2& pos);
		void SetLocation(float x, float y) {
			SetLocation(JVector2(x, y));
		}


		void OffsetLocalLocation(const JVector2& offset);
		void OffsetLocalLocation(float x, float y) {
			OffsetLocalLocation(JVector2(x, y));
		}
		void OffsetLocation(const JVector2& offset);
		void OffsetLocation(float x, float y) {
			OffsetLocation(JVector2(x, y));
		}

		void SetSize(const JVector2& size);
		void SetSize(float x, float y) {
			SetSize(JVector2(x, y));
		}
		void OffsetSize(const JVector2& offset);
		void OffsetSize(float x, float y) {
			OffsetSize(JVector2(x, y));
		}

		void SetAnchor(const JRect&   anchor);
		void SetAnchor(float left, float right, float top, float bottom) {
			SetAnchor(JRect(left, top, right, bottom));
		}
		void SetAnchor(AnchorPreset   preset);
		void SetPivot(const JVector2& pivot);
		void SetPivot(float x, float y) {
			SetPivot(JVector2(x, y));
		}



		const JVector2& GetLocalLocation() const;
		const JVector2& GetLocation() const;

		const JVector2& GetSize() const;

		const JRect& GetAnchor() const;
		const JVector2& GetPivot() const;



		JRect GetLocalRect() const;
		JRect GetRect();



		void  SetParent(Transform* transform);
		Transform* GetParent() const {
			return m_Parent;
		}
		Transform* GetChild(const std::wstring& name) const;
		Transform* GetChild(uint32_t idx) const;
		uint32_t GetChildCount() const {
			return (uint32_t)m_Childs.size();
		}

		void SortChild(const std::function<bool(Transform*, Transform*)>& sortFunc) {
			std::sort(m_Childs.begin(), m_Childs.end(), sortFunc);
		}

		void SwapChild(uint32_t left_index, uint32_t right_index)
		{
			std::swap(m_Childs[left_index], m_Childs[right_index]);
		}

		const JMatrix& GetWorldMatrix() const;
		bool IsDirty() {
			return m_IsWorldUpdateDirty;
		}
		void ClearDirty(bool is_update_matrix = true) {
			if (is_update_matrix) UpdateWorldMatrix();
			else m_IsWorldUpdateDirty = false;
		}
	private:
		void SendDirty(bool is_location_self = true) {
			m_IsWorldUpdateDirty = true;
			if (is_location_self)
			{
				m_IsLocationDirty = true;
			}
			for (auto& child : m_Childs)
			{
				child->SendDirty();
			}
		}

		void CalcAnchorDistance();
		void AnchorProcess();
		void UpdateLocation() const;
		void UpdateWorldMatrix() const;
	protected:
		JVector2 m_LocalLocation;
		mutable JVector2 m_Location;

		JVector2 m_Size;
		
		JRect    m_Anchor = { 0.0f,0.0f,0.0f,0.0f };
		JVector2 m_Pivot = { 0.0f,0.0f };
		
		Transform* m_Parent = nullptr;
		std::vector<Transform*> m_Childs;


		mutable JMatrix m_WorldMatrix = JMatrix::Identity();
		mutable bool    m_IsWorldUpdateDirty   = true;
		mutable bool    m_IsLocationDirty      = true;
		JRect m_DistanceFromAnchor;



		std::shared_mutex m_Mutex;
		std::shared_mutex m_SizeMutex;
	};




}
