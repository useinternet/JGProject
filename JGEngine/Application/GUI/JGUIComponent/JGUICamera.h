#pragma once


#include "JGUIWindowComponent.h"

namespace RE
{
	class FixedGShaderModuleClone;
	class ReCamera;
}
class JGUICamera : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
public:
	void SetLens(float width, float height);
	const JVector2& GetPosition() const;
	void OffsetPosition(float x, float y);
	void OffsetPosition(const JVector2& offset);
	void SetPosition(const JVector2& view_pos);
	void SetPosition(float x, float y);


	JVector2 GetSize() const;


	JVector2 ConvertScreenToWorldPos(const JVector2& pos);













	RE::FixedGShaderModuleClone* GetShaderModule() const;
	const std::string& GetTextureCacheKey() const;
	uint64_t GetOwnerID() const {
		return m_OwnerID;
	}
private:
	std::shared_ptr<RE::FixedGShaderModuleClone> m_GUIModule;
	std::shared_ptr<RE::ReCamera>                m_ReCamera;
	JVector2                                     m_ViewPos;

	class JGUITransform* m_OwnerTransform = nullptr;
	uint64_t             m_OwnerID = 0;
};