#pragma once


#include "CommonCore.h"
#include "GraphicsIF.h"


#include "GUIDefine.h"
#include "GUIIF.h"

#include <string>



namespace GUI
{
	class GUI_API GUIObject
	{
		friend class GUIIF;
	public:
		const std::wstring& GetName() const {
			return m_Name;
		}
		virtual void SetName(const std::wstring& name) {
			m_Name = name;
		}
		uint64_t GetInstanceID() const {
			return m_InstanceID;
		}
		bool IsAlive() const {
			return m_IsAlive;
		}
		virtual ~GUIObject() {}
	private:
		std::wstring m_Name;
		uint64_t     m_InstanceID = 0;
		bool         m_IsAlive    = true;
	};


}