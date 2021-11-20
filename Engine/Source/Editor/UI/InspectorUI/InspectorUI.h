#pragma once
#include "JGCore.h"
#include "ExternalImpl/ImGuiExternal.h"

namespace JG
{
	class IInspectorUI
	{
	public:
		virtual bool OnGUI(IJGObject* _object, bool isForceGUI = false) = 0;
		virtual Type GetObjectType() const = 0;
	public:
		virtual ~IInspectorUI() = default;
	};

	template<class ObjectType>
	class InspectorUI : public IInspectorUI
	{
	public:
		virtual bool OnGUI(IJGObject* _object, bool isForceGUI) override
		{

			if (isForceGUI)
			{
				auto asObject = static_cast<ObjectType*>(_object);
				OnGUI_Impl(asObject);
				return true;
			}
			else
			{
				if (GetObjectType() == _object->GetType())
				{
					auto asObject = _object->As<ObjectType>();
					if (asObject != nullptr)
					{
						OnGUI_Impl(asObject);
						return true;
					}
				}
			}
			return false;
		}
		

		virtual Type GetObjectType() const override {
			return JGTYPE(ObjectType);
		}

	protected:
		virtual void OnGUI_Impl(ObjectType* _object) {}
	public:
		virtual ~InspectorUI() = default;
	};

}