#include "pch.h"
#include "JGUICamera.h"
#include "JGUITransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"

#include "RenderEngine.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/ReObject/ReCamera.h"
#include "Object/DxObject/RenderDevice.h"
using namespace std;

void JGUICamera::Awake()
{

	m_ReCamera = make_shared<RE::ReCamera>();

	if (m_ElementOwner)  m_OwnerID = m_ElementOwner->GetID();
	if (m_WindowOwner)   m_OwnerID = m_WindowOwner->GetID();


	if (m_ElementOwner)  m_OwnerTransform = m_ElementOwner->GetTransform();
	if (m_WindowOwner)   m_OwnerTransform = m_WindowOwner->GetTransform();
	RE::RenderEngine::RegisterRIManager(m_OwnerID);
}

void JGUICamera::Destroy()
{
	RE::RenderEngine::UnRegisterTexture(m_GUIModule->GetTextureCacheKey());
	m_GUIModule = nullptr;
	RE::RenderEngine::UnRegisterRIManager(m_OwnerID);
}

void JGUICamera::Tick(float tick)
{
	auto device = RE::RenderEngine::GetDevice();
	if (m_GUIModule)
	{
		device->SubmitToRender(JGUI_SubmitCmdListPriority_Default, [&](RE::CommandList* cmdList)
		{
			if (m_GUIModule == nullptr) return;


			m_GUIModule->Execute(cmdList);

		});
	}
}




void JGUICamera::SetLens(float width, float height)
{
	uint64_t id = m_OwnerID;
	
	if (m_ReCamera == nullptr)
	{
		m_ReCamera = make_shared<RE::ReCamera>();

	}
	width  = std::max<float>(10, width);
	height = std::max<float>(10, height);
	m_ReCamera->SetLens(45, width, height);
	m_ReCamera->SetPosition({ m_ViewPos.x,m_ViewPos.y,-10.0f });
	m_ReCamera->ConvertOrthographic();

	m_GUIModule = RE::RenderEngine::GetGUIModule()->Clone(m_ReCamera.get(), id);

	RE::RenderEngine::UnRegisterTexture(m_GUIModule->GetTextureCacheKey());
	RE::RenderEngine::RegisterTexture(m_GUIModule->GetTextureCacheKey(), m_GUIModule->GetRTTexture(0));
}
const JVector2& JGUICamera::GetPosition() const
{
	return m_ViewPos;
}
void JGUICamera::OffsetPosition(float x, float y)
{
	m_ViewPos.x += x;
	m_ViewPos.y += y;
	m_ReCamera->SetPosition(m_ViewPos.x, m_ViewPos.y, -10);
	if (m_OwnerTransform) m_OwnerTransform->SendDirty();
	m_GUIModule->BindCamera(m_ReCamera.get());
}
void JGUICamera::OffsetPosition(const JVector2& offset)
{
	m_ViewPos += offset;
	m_ReCamera->SetPosition(m_ViewPos.x, m_ViewPos.y, -10);

	if (m_OwnerTransform) m_OwnerTransform->SendDirty();
	m_GUIModule->BindCamera(m_ReCamera.get());
}
void JGUICamera::SetPosition(const JVector2& view_pos)
{
	m_ViewPos = view_pos;
	m_ReCamera->SetPosition(m_ViewPos.x, m_ViewPos.y, -10);

	if (m_OwnerTransform) m_OwnerTransform->SendDirty();
	m_GUIModule->BindCamera(m_ReCamera.get());
}
void JGUICamera::SetPosition(float x, float y)
{
	m_ViewPos.x = x;
	m_ViewPos.y = y;
	m_ReCamera->SetPosition(m_ViewPos.x, m_ViewPos.y, -10);

	if (m_OwnerTransform) m_OwnerTransform->SendDirty();
	m_GUIModule->BindCamera(m_ReCamera.get());
}

JVector2 JGUICamera::GetSize() const
{
	return { m_ReCamera->GetLensWidth(), m_ReCamera->GetLensHeight() };
}

JVector2 JGUICamera::ConvertScreenToWorldPos(const JVector2& pos)
{
	JVector2 result;
	auto size = GetSize();
	result.x =  pos.x - (size.x * 0.5f);
	result.y = -pos.y + (size.y * 0.5f);

	return result;
}



RE::FixedGShaderModuleClone* JGUICamera::GetShaderModule() const
{
	return m_GUIModule.get();
}

const std::string& JGUICamera::GetTextureCacheKey() const
{
	return m_GUIModule->GetTextureCacheKey();
}
