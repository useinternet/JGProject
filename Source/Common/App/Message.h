#pragma once

#include <map>
#include <queue>
#include <shared_mutex>
#include "Data/Builder.h"
#include "Math/JVector.h"

#define GRAPHICS_ENGINE_MESSAGE_ID "GraphicsEngine"
#define GFW_MESSAGE_ID             "GameFrameWork"
#define INPUT_ENGINE_MESSAGE_ID    "InputEngine"

class SystemCore;
class Application;
class Message
{
public:
	void*    data      = nullptr;
	uint64_t dataSize  = 0;
	uint64_t msgID     = -1;
	uint64_t receiptID = UINT64_MAX;
};

class MessageBus
{
	friend Application;
	friend SystemCore;
public:
	/*
	function : 메시지가 전달되지만, 언제 도착할지는 알수 없습니다. */
	void Post(const Message& msg);
	/*
    function : 메시지가 즉시 전달됩니다. */
	void Send(const Message& msg);

	/*
    function : 등록된 ID 얻어오기 */
	uint64_t GetID(const std::string& name) const;
private:
	/*
	function : Post로 온 메시지를 처리힙니다. */
	void Clear();

	/*
	function : ID 발급*/
	uint64_t IssueID(const std::string& name, SystemCore* sys);
private:
	std::queue<Message>        m_MsgQueue;
	mutable std::shared_mutex  m_MsgMutex;

	// 
	std::map<uint64_t, SystemCore*> m_Systems;
	std::map<std::string, uint64_t> m_IDMap;
	uint64_t m_IDOffset = 1;
	//
	uint32_t m_ProcessMessagePerFrame = 0;
	uint32_t m_AllowMaxProcessFrame = 5;
	uint32_t m_AllowMinProcessFrame = 2;
};






namespace GraphicsMessage
{
	enum 
	{
		Msg_BeginDraw,
		Msg_EndDraw,
	};
}

namespace GameFrameWorkMessage
{
	using GFWInstanceAddr = uint64_t;
	enum
	{
		Msg_RequestEditorCamera,
		Msg_GetSelectGameObject,
		Msg_TestSpawnGameObject




		// Msg_GetWorldInfo,

	};

	struct RequestEditorCamera
	{
	/* Msg_RequestEditorCamera
	* 에디터 카메라 인스턴스 가져오기
	* Snd
	std::wstring        -> 월드 이름
	* Rcv
	GE::SceneCamera*    -> 씬 카메라
	GE::Texture*        -> 씬 텍스쳐
	*/
		struct Snd
		{
			std::wstring GameWorldName;
		};
		struct Rcv
		{
			GFWInstanceAddr SceneCameraAddr;
			GFWInstanceAddr SceneTextureAddr;
		};

		Snd snd;
		Rcv rcv;
	};

	struct GetSelectGameObject
	{
	/* Msg_GetSelectGameObject
	* 설명 : 선택된 게임 오브젝트에 대한 정보 얻기
	* Snd
	JVector2   -> 마우스 위치
	
	* Rcv
	
	GameObject에 대한 정보
	{
		Name
		classID
		EditorLayoutBuilderFunc
		EditorDataBindBuilderFunc
		GameObject 조작할수있는 Pointer
	}
	
	*/
		struct Snd
		{
			JVector2 mousePos;
		};
		struct Rcv
		{
			std::wstring Name;
			std::string  ClassID;
			EditorLayoutBuilder   LayoutBuilder;
			EditorDataBindBuilder DataBindBuilder;

		};

		Snd snd;
		Rcv rcv;


	};



	/* Msg_TestSpawnGameObject
	* Snd
	meshPath = "";
	matName  = "";
	initlocation;
	initRotation;
	initScale;



	* Rcv
	
	
	*/
	/* Msg_SpawnGameObject
	* 미구현(추후 구현 예정)
	* Snd
	



	* Rcv
	
	
	*/
}

// 0000 0000 0000 0000
// 0000 0000 0000 0000
// 0000 0000 0000 0000
// 0000 0000 0000 0000