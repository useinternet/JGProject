#pragma once

#include "RE.h"
#include "Defines/ReEnumDefines.h"
#include "Defines/ReDataTypeDefines.h"
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace RE
{
	class RenderDevice;
	class ShaderLibManager;
	class RenderItemManager;
	class ShaderModuleManager;
	class RENDERENGINE_API ReObject
	{
		friend RenderDevice;
		friend ShaderLibManager;
		friend RenderItemManager;
		friend ShaderModuleManager;
	public:
		ReObject() = default;
		ReObject(const std::string& name) : m_Name(name) { }
	
		virtual ~ReObject() = default;

	public:
		virtual void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }


		RenderDevice* GetRenderDevice() {
			return m_RenderDevice;
		}
		RenderDevice* GetRenderDevice() const {
			return m_RenderDevice;
		}
		ShaderLibManager* GetShaderLibManager() {
			return m_ShaderLibManager;
		}
		ShaderLibManager* GetShaderLibManager() const {
			return m_ShaderLibManager;
		}
		RenderItemManager* GetRenderItemManager()  {
			return m_RenderItemManager;
		}
		RenderItemManager* GetRenderItemManager() const {
			return m_RenderItemManager;
		}
		ShaderModuleManager* GetShaderModuleManager() {
			return m_ShaderModuleManager;
		}
		ShaderModuleManager* GetShaderModuleManager() const {
			return m_ShaderModuleManager;
		}
	private:
		std::string m_Name;
		static RenderDevice*      m_RenderDevice;
		static ShaderLibManager*  m_ShaderLibManager;
		static RenderItemManager* m_RenderItemManager;
		static ShaderModuleManager* m_ShaderModuleManager;
	};
}