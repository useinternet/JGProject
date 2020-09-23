#pragma once
#include "GFWCore.h"






namespace GFW
{
	class GAMEFRAMEWORK_API GFWObject
	{
	
	public:
		GFWObject(const std::wstring& name) : m_Name(name) {}
		virtual ~GFWObject() = default;
	public:
		const std::wstring& GetName() const {
			return m_Name;
		}
		void SetName(const std::wstring& name) {
			m_Name = name;
		}
	protected:
		virtual void Awake()   = 0;
		virtual void Start()   = 0;
		virtual void Tick()    = 0;
		virtual void Destroy() = 0;
	public:
		std::wstring m_Name;
	};
}