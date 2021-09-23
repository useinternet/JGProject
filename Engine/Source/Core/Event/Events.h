#pragma once
#include "Common/Define.h"
#include "Common/Enum.h"
#include "Common/Type.h"
#include "Common/Abstract.h"
#include "Common/Color.h"
#include "Common/Object.h"
#include "Math/Math.h"
#include "Math/JVector.h"
#include "Math/JRect.h"
#include "Math/JRay.h"
#include "Math/JQuaternion.h"
#include "Math/JPlane.h"
#include "Math/JFrustum.h"
#include "Math/JBBox.h"
#include "Math/JMatrix.h"
#include <string>



namespace JG
{
	ENUM_FLAG(EEventCategory)
	enum class EEventCategory
	{
		None			= 0,
		Application		= 0x001,
		Keyboard		= 0x002,
		Mouse			= 0x004,
		MouseButton		= 0x008,
		Input			= 0x010,

		Request         = 0x100,
		Response        = 0x200,
		Notify			= 0x400,
		Destroy         = 0x800,
	};

#define  EVENTCLASS(className, category) \
public: \
	virtual Type GetEventType()  const override {\
		static Type type;\
		if (type.GetID() == TYPE_NULL_ID)\
		{\
			type = Type(TypeID<##className>());\
		}\
		return type;\
	} \
	virtual EEventCategory GetCategory() const override { return category; } \
	static  Type GetStaticEventType()\
	{\
		static Type type;\
		if (type.GetID() == TYPE_NULL_ID)\
		{\
			type = Type(TypeID<##className>());\
		}\
		return type;\
	} \
private: \


#define EVENT_BIND_FN(func) std::bind(func, this, std::placeholders::_1)

	
	using EventResult = ptraddr;
	class IEvent
	{
	public:
		virtual ~IEvent() = default;
		virtual String    ToString()    const = 0;
		virtual Type      GetEventType()const = 0;
		virtual EEventCategory GetCategory() const = 0;
	public:
		bool IsInCategory(EEventCategory category) const
		{
			return GetCategory() & category;
		}

	public:
		bool	    Handled = false;
		EventResult Result  = 0;
	};

	class EventDispatcher
	{
	private:
		IEvent& mEvent;
	public:
		EventDispatcher(IEvent& e) : mEvent(e) {}
	public:
		template<class T, class Func>
		bool Dispatch(const Func& func)
		{
			if(mEvent.GetEventType() == T::GetStaticEventType())
			{
				mEvent.Handled |= func(static_cast<T&>(mEvent));
				return true;
			}
			return false;
		}
	};





#define CREATE_EVENT_(category, className, toString)\
	class className : public IEvent \
	{ \
	private: \
		EVENTCLASS(className, ##category)\
	public:\
		virtual ~##className() = default; \
		virtual String ToString() const override\
		{\
			return #toString;\
		}\
	};\

#define CREATE_EVENT_ONE_ARG_(category, className, argType_1, argName_1, argInit_1, toString) \
	class className : public IEvent \
	{ \
	private: \
		EVENTCLASS(className, ##category)\
	public: \
		argType_1 argName_1 = argInit_1; \
	public:\
		virtual ~##className() = default; \
		virtual String ToString() const override\
		{\
			return toString;\
		}\
	};\

#define CREATE_EVENT_TWO_ARG_(category, className, argType_1, argName_1, argInit_1, argType_2, argName_2, argInit_2, toString) \
	class className : public IEvent \
	{ \
	private: \
		EVENTCLASS(className, ##category)\
	public: \
		argType_1 argName_1 = argInit_1; \
		argType_2 argName_2 = argInit_2; \
	public:\
		virtual ~##className() = default; \
		virtual String ToString() const override\
		{\
			return toString;\
		}\
	};\

#define CREATE_EVENT(category, className) CREATE_EVENT_(category, className, #className) 
#define CREATE_EVENT_ONE_ARG(category, className, argType_1, argName_1, argInit_1) CREATE_EVENT_ONE_ARG_(category, className, argType_1, argName_1,argInit_1, #className)
#define CREATE_EVENT_TWO_ARG(category, className, argType_1, argName_1, argType_2, argName_2) CREATE_EVENT_TWO_ARG_(category, className, argType_1, argName_1, argInit_1, argType_2, argName_2,argInit_2, #className)




#define NOTIFY_EVENT(className) CREATE_EVENT(EEventCategory::Notify, className)
#define NOTIFY_ONE_ARG_EVENT(className, argType_1, argName_1, argInit_1) CREATE_EVENT_ONE_ARG(EEventCategory::Notify, className, argType_1, argName_1,argInit_1)
#define REQUEST_EVENT(className) CREATE_EVENT(EEventCategory::Request, className)
#define REQUEST_ONE_ARG_EVENT(className, argType_1, argName_1, argInit_1) CREATE_EVENT_ONE_ARG(EEventCategory::Request, className, argType_1, argName_1, argInit_1)




	// Notify
	class IJGObject;
	class ITexture;
	class GameWorld;
	class IRenderItem;
	class ILightItem;
	class Camera;
	class GameNode;
	class IAsset;
	class AssetID;

	NOTIFY_EVENT(NotifyRenderingReadyCompeleteEvent)
	NOTIFY_ONE_ARG_EVENT(NotifyDestroyJGObjectEvent, IJGObject*, DestroyedObject, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifyChangeMainSceneTextureEvent, SharedPtr<ITexture>, SceneTexture, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifyChangeGameWorldEvent, GameWorld*, GameWorld, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifyEditorSceneOnClickEvent, JVector2, ClickPos, JVector2(0, 0))
	NOTIFY_ONE_ARG_EVENT(NotifySelectedGameNodeInEditorEvent, GameNode*, SelectedGameNode, nullptr)
	NOTIFY_ONE_ARG_EVENT(NotifySelectedAssetInEditorEvent, String, SelectedAssetPath, "")

	REQUEST_EVENT(RequestSaveGameWorldEvent)
	REQUEST_ONE_ARG_EVENT(RequestLoadGameWorldEvent, String, AssetPath, "")
	REQUEST_ONE_ARG_EVENT(RequestPushRenderItemEvent, SharedPtr<IRenderItem>, RenderItem, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestPushLightItemEvent, SharedPtr<ILightItem>, LightItem, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestRegisterCameraEvent, Camera*, pCamera, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestUnRegisterCameraEvent, Camera*, pCamera, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestRegisterMainCameraEvent, Camera*, MainCamera, nullptr)
	REQUEST_ONE_ARG_EVENT(RequestUnRegisterMainCameraEvent, Camera*, MainCamera, nullptr)
}