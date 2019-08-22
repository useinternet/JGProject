#pragma once

#include "RE.h"
#include "Defines/ReEnumDefines.h"
#include "Defines/ReDataTypeDefines.h"
#include <string>
#include <d3d12.h>
#include <wrl.h>

namespace RE
{
	class RenderDevice;
	class RENDERENGINE_API ReObject
	{
		friend RenderDevice;
	public:
		ReObject() = default;
		ReObject(const std::string& name) : m_Name(name) { }
	
		virtual ~ReObject() = default;

	public:
		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }


		RenderDevice* GetRenderDevice() {
			return m_RenderDevice;
		}
		RenderDevice* GetRenderDevice() const {
			return m_RenderDevice;
		}
	private:
		std::string m_Name;
		static RenderDevice* m_RenderDevice;
	};
}