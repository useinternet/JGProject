#pragma once
#include "Core.h"
#include "Components/NativeScriptComponent.h"
#include <array>
namespace JGUI
{
	class ImageComponent;
	class TextMeshComponent;
	class Dropdown;
	class Element;
	class Button;
	class Slider;
	class GUIDropItem;
	class TreeView;
	class TreeItem;
}
/*
직각 투영
DXGI_FORMAT_D24_UNORM;
*/
class UnitTest : public JGUI::Form
{

protected:
	virtual void Awake()   override;
	virtual void Start()   override;
	virtual void Tick()    override;
	virtual void Destroy() override;

	virtual void OnKeyUp(KeyCode code) override;
	virtual void OnKey(KeyCode code) override;
	virtual void OnMouseClick(KeyCode code) override;
	virtual void OnChar(wchar_t c) override;
	// OnDropItem(const GUIDropItem& item)
	virtual void OnDropItem(const JGUI::GUIDropItem& item) override;
public:
	class Unit
	{
	public:
		std::wstring Name;
		std::wstring MeshName;
		std::vector<SceneObjectRef>                sceneObjectRef;
		std::vector<MeshGenerator::StaticMeshData> staticMeshDatas;
		std::vector<MaterialRef>                   materialRef;
		JBBox    BoundingBox;
		SDObject Object;
		JVector3 Location;
		JVector3 Rotation;
		JVector3 Scale = { 1.0f,1.0f,1.0f };
		bool Dirty = true;


		std::vector<GE::SceneObject*>             submitObjects;
	};
	struct LightInfo
	{
		JColor   Color = JColor::White();
		JVector3 Location;
		JVector3 Rotation;
		float att0 = 1.0f;
		float att1 = 0.0f;
		float att2 = 0.0f;
		float inRange = 100.0f;
		float outRange = 200.0f;
		float inCone = 15.0f;
		float outCone = 30.0f;
	};
	class Light
	{
	public:
		std::wstring name;
		JColor   Color = JColor::White();
		JVector3 Location;
		JVector3 Rotation;
		float att0 = 1.0f;
		float att1 = 0.0f;
		float att2 = 0.0f;
		float inRange = 100.0f;
		float outRange = 200.0f;
		float inCone = 15.0f;
		float outCone = 30.0f;
		GE::SceneLight* ptr = nullptr;
	private:
		friend UnitTest;
		DirectionalLightRef dref;
		PointLightRef pref;
		SpotLightRef  sref;

	};
public:
	void  CreateUnit(const std::wstring& name,  std::wstring meshName, const std::wstring& initm);
	void  CreateUnit(const std::wstring& name, std::wstring meshName, const std::vector<std::wstring>& m);
	void CreateDirLight(const std::wstring& name);
	void CreatePointLight(const std::wstring& name);
	void CreateSpotLight(const std::wstring& name);
	UnitTest::Unit* GetUnit(const std::wstring& name);
	UnitTest::Light* GetLight(const std::wstring& name);
	std::map<std::wstring, std::unique_ptr<Unit>>  UnitPool;
	std::map<std::wstring, std::unique_ptr<Light>> LightPool;
	std::unordered_map<std::wstring, std::vector<MeshGenerator::StaticMeshData>> MeshPool;
private:
	void InitMaterial();

public:
	bool IsDebugMode = false;
	JGUI::ImageComponent* TestScene = nullptr;
	class Inspector* InspectorForm = nullptr;
	class Spawner* SpawnerForm = nullptr;
	class MapSaveLoad* MapLoader = nullptr;
	class WorldOutliner* WorldOutLiner = nullptr;

	// 
	SceneRef        RESceneRef;
	GE::SceneCamera RESceneCamera;

	std::array<JGUI::TextMeshComponent*, 10> DebugTextLine;


	bool     CameraMode = false;
	JVector2 CursorPos;
	float CameraSensitivity = 240.0f;
	float CameraSpeed       = 18.0f;

	Unit* SelectedUnit = nullptr;
	Light* SelectedLight = nullptr;
	bool IsDirLight = false;
};
// 맵 로더
// 월드 현황판
class Inspector : public JGUI::Form
{
protected: // 0.01 0.0001
	virtual void Awake()   override;
	virtual void Start()   override;
	virtual void Tick()    override;
	virtual void Destroy() override;
public:
	void SetUnitInspector(UnitTest::Unit* unit);
	void SetLightInspector(UnitTest::Light* light);
	struct ObjectDetail
	{
		class JVector3Information* Location = nullptr;
		class JVector3Information* Rotation = nullptr;
		class JVector3Information* Scale = nullptr;
		class DropdownInformation* MaterialSlot = nullptr;
		std::unordered_map<std::wstring, JGUI::Element*> MaterialSlotByModel;
	};
	struct DirectionalLightDetail
	{
		class JVector3Information* Dir   = nullptr;
		class JVector4Information* Color = nullptr;
	};
	struct PointLightDetail
	{
		class JVector3Information* Location = nullptr;
		class JVector4Information* Color = nullptr;
		class FloatInformation* att0 = nullptr;
		class FloatInformation* att1 = nullptr;
		class FloatInformation* att2 = nullptr;
		class FloatInformation* inRange = nullptr;
		class FloatInformation* outRange = nullptr;

	};
	struct SpotLightDetail
	{
		class JVector3Information* Dir = nullptr;
		class JVector3Information* Location = nullptr;
		class JVector4Information* Color = nullptr;
		class FloatInformation* att0 = nullptr;
		class FloatInformation* att1 = nullptr;
		class FloatInformation* att2 = nullptr;
		class FloatInformation* inRange = nullptr;
		class FloatInformation* outRange = nullptr;
		class FloatInformation* inCone = nullptr;
		class FloatInformation* outCone = nullptr;
	};

	UnitTest* unitTest = nullptr;
private:
	class JGUI::Button* DeleteButton = nullptr;
	class TextInformation* ObjectType = nullptr;
	class TextInformation* ObjectName = nullptr;
	UnitTest::Unit*  SelectedUnit  = nullptr;
	UnitTest::Light* SelectedLight = nullptr;
	std::map<std::wstring, JGUI::Element*> InspectorByType;
	ObjectDetail _ObjectDetail;
	DirectionalLightDetail _DLDetail;
	PointLightDetail _PLDetail;
	SpotLightDetail  _SLDetail;
	int  PrevIndex = -1;
};


class Spawner : public JGUI::Form
{
protected:
	virtual void Awake()   override;
	virtual void Start()   override;
	virtual void Tick()    override;
	virtual void Destroy() override;

	virtual void OnKeyUp(KeyCode code) override;
	enum
	{
		Block_InputText,
		Block_Dropdown,
		Block_TextBox,
		Block_Texture,
	};
private:
	JGUI::Element* AddBlock(const std::wstring& name, JGUI::Element* element,  int uiType, float& BlockOffsetY);
	JGUI::Element* CreateUIByMaterialModel(JGUI::Element* ownerElement,const std::wstring& slotName);
public:
	// 오브젝트 타입
	// 유닛 이름
	// 메시
	// 초기화 머터리얼 모델
	// 
	UnitTest* unitTest = nullptr;
	JGUI::Dropdown*     ObjectType = nullptr;
	class InputText*    UnitNameInput = nullptr;

	// Mesh
	JGUI::TextMeshComponent* MeshPath = nullptr;

	// Material
	//  
	float MaterialPanelOffsetY = 0.0f;
	JGUI::Element*                     MaterialPanel = nullptr;
	std::vector<JGUI::Element*>        MaterialInspectors;
	std::vector<JGUI::Dropdown*>       MaterialDropdowns;
	std::vector<JGUI::ImageComponent*> MaterialBackGround;



	/*
	Dropdown
		inspector
			Default
			Texture
				Albedo
					Text
					Texture
			NomalMapping
				Albedo
					Text
					Texture
				Normal
					Text
					Texture

	
	*/






	JGUI::Button* CreateBt = nullptr;



};
class WorldOutliner : public JGUI::Form
{
protected:
	virtual void Awake()   override;
	virtual void Start()   override;
	virtual void Tick()    override;
	virtual void Destroy() override;
public:
	void Reset();
public:
	JGUI::TreeView* WorldTreeView = nullptr;
	JGUI::TreeItem* Root = nullptr;
	JGUI::TreeItem* ObjectTreeItem = nullptr;
	JGUI::TreeItem* PointLightItem = nullptr;
	JGUI::TreeItem* SpotLightTreeItem = nullptr;
	JGUI::TreeItem* DirectionalLightTreeItem = nullptr;


	UnitTest* unitTest;
	UnitTest::Unit* SelectUnit = nullptr;
	UnitTest::Unit* PrevUnit   = nullptr;

	UnitTest::Light* SelectLight = nullptr;
	UnitTest::Light* PrevSelectLight = nullptr;

//	JGUI::TreeView* ObjectTreeView = nullptr;
};
class MapSaveLoad : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
public:
	void Load(const std::wstring& path);
	void Save(const std::wstring& path);

	void Write(std::ofstream& fout, const std::wstring& str);
	void Read(std::ifstream& fin, std::wstring& str);
public:
	UnitTest* unitTest = nullptr;
public:
	class InputTextField* Input = nullptr;
	JGUI::Button* SaveBt          = nullptr;
};
class InputText : public JGUI::NativeScriptComponent
{
protected:
	virtual void Start() override;

public:
	JGUI::ImageComponent*    image = nullptr;
	JGUI::TextMeshComponent* text  = nullptr;
};

class TextureLoader : public JGUI::NativeScriptComponent
{
protected:
	virtual void Start() override;

public:
	JGUI::ImageComponent* image = nullptr;
};

class MaterialSlot : public JGUI::NativeScriptComponent
{
protected:
	virtual void Start() override;

public:
	JGUI::ImageComponent* image = nullptr;
};

class TextInformation : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
public:
	JGUI::TextMeshComponent* Name     = nullptr;
	JGUI::TextMeshComponent* Contents = nullptr;
};
class FloatInformation : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
	virtual void Tick() override;
public:
	JGUI::TextMeshComponent* Name = nullptr;
	JGUI::Slider*            FloatSlider = nullptr;
	JGUI::TextMeshComponent* Value = nullptr;
};


class InputTextField : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;

public:
	bool IsOnlyNumber = false;
	JGUI::ImageComponent*    BackGround = nullptr;
	JGUI::TextMeshComponent* Text    = nullptr;
	bool IsFocusing = false;
};

class JVector4Information : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
public:
	void SetValue(const JVector4& v);
	const JVector4& GetValue() const;
	bool IsRevising() const;
public:
	JGUI::TextMeshComponent* Name = nullptr;
	JGUI::TextMeshComponent* XName = nullptr;
	InputTextField* XInput = nullptr;
	JGUI::TextMeshComponent* YName = nullptr;
	InputTextField* YInput = nullptr;
	JGUI::TextMeshComponent* ZName = nullptr;
	InputTextField* ZInput = nullptr;
	JGUI::TextMeshComponent* WName = nullptr;
	InputTextField* WInput = nullptr;
private:
	JVector4 value;
};

class JVector3Information : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
public:
	void SetValue(const JVector3& v);
	const JVector3& GetValue() const;
	bool IsRevising() const;
public:
	JGUI::TextMeshComponent* Name = nullptr;
	JGUI::TextMeshComponent* XName = nullptr;
	InputTextField* XInput = nullptr;
	JGUI::TextMeshComponent* YName = nullptr;
	InputTextField* YInput = nullptr;
	JGUI::TextMeshComponent* ZName = nullptr;
	InputTextField* ZInput = nullptr;
private:
	JVector3 value;
};

class DropdownInformation : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
public:
	JGUI::TextMeshComponent* Name     = nullptr;
	JGUI::Dropdown*          Dropdown = nullptr;
};

class TextureInformation : public JGUI::NativeScriptComponent
{
protected:
	virtual void Awake() override;
public:
	JGUI::TextMeshComponent* Name = nullptr;
	JGUI::ImageComponent* Texture = nullptr;
};