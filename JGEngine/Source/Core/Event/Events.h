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
	//enum class EEventType
	//{
	//	// Application
	//	AppOpen, AppClose,  AppResize,

	//	// Input
	//	KeyPressed, KeyReleased, KeyTyping,
	//	MouseMoved, MouseButtonPressed, MouseButtonReleased,
	//	MouseScrollMoved,

	//	// Game
	//};


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

#define  EVENTCLASS(category) \
public: \
	virtual Type GetEventType()  const override { return Type(TypeID(this)); } \
	virtual EEventCategory GetCategory() const override { return category; } \
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
			if(mEvent.GetEventType() == Type(TypeID<T>()))
			{
				mEvent.Handled |= func(static_cast<T&>(mEvent));
				return true;
			}
			return false;
		}
	};


#define CREATE_EVENT(category, className) \
	class className : public IEvent \
	{ \
		EVENTCLASS(##category)\
	public:\
		virtual ~##className() = default; \
		virtual String ToString() const override\
		{\
			return #className;\
		}\
	};\

#define CREATE_EVENT_ONE_ARG(category, className, argType_1, argName_1) \
	class className : public IEvent \
	{ \
		EVENTCLASS(##category)\
	public: \
		argType_1 argName_1; \
	public:\
		virtual ~##className() = default; \
		virtual String ToString() const override\
		{\
			return #className;\
		}\
	};\




#define NOTIFY_EVENT(className) CREATE_EVENT(EEventCategory::Notify, className)
#define NOTIFY_ONE_ARG_EVENT(className, argType_1, argName_2) CREATE_EVENT_ONE_ARG(EEventCategory::Notify, className, argType_1, argName_2)
#define REQUEST_ONE_ARG_EVENT(className, argType_1, argName_2) CREATE_EVENT_ONE_ARG(EEventCategory::Request, className, argType_1, argName_2)




	// Notify
	class ITexture;
	class GameWorld;
	class IRenderItem;
	class Camera;
	class GameNode;
	class IAsset;
	class AssetID;
	NOTIFY_EVENT(NotifyRenderingReadyCompeleteEvent)
	NOTIFY_ONE_ARG_EVENT(NotifyChangeMainSceneTextureEvent, SharedPtr<ITexture>, SceneTexture)
	NOTIFY_ONE_ARG_EVENT(NotifyChangeGameWorldEvent, GameWorld*, GameWorld)
	NOTIFY_ONE_ARG_EVENT(NotifyEditorSceneOnClickEvent, JVector2, ClickPos)
	NOTIFY_ONE_ARG_EVENT(NotifySelectedGameNodeInEditorEvent, GameNode*, SelectedGameNode)
	NOTIFY_ONE_ARG_EVENT(NotifySelectedAssetInEditorEvent, SharedPtr<AssetID>, SelectedAsset)
		
	REQUEST_ONE_ARG_EVENT(RequestGetGameWorldEvent, GameWorld*, GameWorld)
	REQUEST_ONE_ARG_EVENT(RequestPushRenderItemEvent, SharedPtr<IRenderItem>, RenderItem)
	REQUEST_ONE_ARG_EVENT(RequestRegisterCameraEvent, Camera*, pCamera)
	REQUEST_ONE_ARG_EVENT(RequestUnRegisterCameraEvent, Camera*, pCamera)
	REQUEST_ONE_ARG_EVENT(RequestRegisterMainCameraEvent, Camera*, MainCamera)
	REQUEST_ONE_ARG_EVENT(RequestUnRegisterMainCameraEvent, Camera*, MainCamera)
}