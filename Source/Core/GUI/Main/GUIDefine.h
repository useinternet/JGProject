#pragma once


#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <shared_mutex>
#include "GraphicsIF.h"
#ifdef GUI_EXPORT

#define GUI_API __declspec(dllexport)

#else

#define GUI_API __declspec(dllimport)

#endif
#define GUI_MATERIAL_DATA_COLOR   "Color"
#define GUI_MATERIAL_DATA_IMAGE   "Image"
#define GUI_PREGENERATED_MATERIAL_DEFAULT TT("GUI_PreGenerated_Material_Default")
#define GUI_PREGENERATED_MATERIAL_IMAGE   TT("GUI_PreGenerated_Material_Image")
#define GUI_PREGENERATED_MATERIAL_TEXT    TT("GUI_PreGenerated_Material_Text")
namespace JGUI
{
    enum ElementFlags
    {
        ElementFlag_None    = 0x0000,
		ElementFlag_TopMost = 0x0001,
		ElementFlag_AboveOnWindow = 0x0002
    };
    enum WindowFlags
    {
        WindowFlag_None       = 0x0000,
        WindowFlag_RootWindow = 0x00001,
        WindowFlag_NoMove     = 0x00002,
        WindowFlag_Border     = 0x00004,
        WindowFlag_TitleBar   = 0x00008,
        WindowFlag_Resize     = 0x00010,
		WindowFlag_CloseBt    = 0x00020,
		WindowFlag_HideBt     = 0x00040,
		WindowFlag_MaxBt      = 0x00080,


		WindowFlag_HideMaxCloseBt = WindowFlag_CloseBt | WindowFlag_HideBt | WindowFlag_MaxBt,



        WindowFlag_MainWindow     = WindowFlag_RootWindow | WindowFlag_Resize | WindowFlag_Border | WindowFlag_TitleBar | WindowFlag_HideMaxCloseBt,
		WindowFlag_ChildWindow    = WindowFlag_Border | WindowFlag_Resize | WindowFlag_TitleBar | WindowFlag_CloseBt
    };
    enum GUIIOFlags
    {
        GUIIOFlag_None,
        GUIIOFlag_NoMultiSwapChain
    };
    enum GUI_PreGenerated_Material
    {
        GUI_PreGenerated_Material_Default,
        GUI_PreGenerated_Material_Image,
        GUI_PreGenerated_Material_Text,
    };
    
    enum GUIDraw_Priority : uint64_t
    {
        GUIDraw_Priority_WindowBackGround   = 0,
        GUIDraw_Priority_BottomMostElement  = 1,
        GUIDraw_Priority_TopMostElement     = 9999999,
		GUIDraw_Priority_TitleBar,
        GUIDraw_Priority_BottomMostWindow   = 50000000,
        GUIDraw_Priority_Window             = 50000001,
        GUIDraw_Priority_TopMostWindow      = 99999999,
        GUIDraw_Priority_WindowResizeGrip,
        GUIDraw_Priority_WindowBorder       ,
    };

    enum TextHAlign
    {
        TextHAlign_Left,
        TextHAlign_Right,
        TextHAlign_Center
    };
    enum TextVAlign
    {
        TextVAlign_Top,
        TextVAlign_Bottom,
        TextVAlign_Center
    };




    enum 
    {
        GUIStyle_WindowBackGround,
        GUIStyle_WindowBorder,
        GUIStyle_WindowResizeGrip,
        GUIStyle_WindowResizeGripHightlight,
		GUIStyle_TitleBar,
		GUIStyle_TitleText,
		GUIStyle_ButtonNormal,
		GUIStyle_ButtonHightlight,
		GUIStyle_ButtonPressed,
		GUIStyle_ButtonSelected,
		GUIStyle_ButtonDisable,

		GUIStyle_SliderHandle,
		GUIStyle_SliderBackGround,
		GUIStyle_SliderFillArea,
        GUIStyle_Count,
    };
	enum 
	{
		GUIStyle_Resource_CloseBt,
		GUIStyle_Resource_MaxBt,
		GUIStyle_Resource_HideBt,
		GUIStyle_Resource_Check,
		GUIStyle_Resource_RightFillArrow,
		GUIStyle_Resource_RightArrow,
		GUIStyle_Resource_LeftFillArrow,
		GUIStyle_Resource_LeftArrow,
		GUIStyle_Resource_UpFillArrow,
		GUIStyle_Resource_UpArrow,
		GUIStyle_Resource_DownFillArrow,
		GUIStyle_Resource_DownArrow,
		GUIStyle_Resource_Count,
	};
	class JFontManager;
	class JFont;
	struct GUIDrawItem
	{
		uint64_t priority = 0;
		GE::SceneObject* object = nullptr;
	};
	struct GUIInteractionItem
	{
		class Element* ownerElement = nullptr;
		std::function<bool()> interactionFunc;
		uint64_t draw_priority = 0;
		bool     prevEnter = false;
	};

	struct GUIWindowDrawItem
	{
		SceneRef        scene;
		GE::SceneCamera camera;
		std::map<uint64_t, std::vector<GE::SceneObject*>> drawItems;



		SceneObjectRef sceneObject;
		std::vector<PaperVertex> v;
		std::vector<uint32_t>    i;
		SDObject instance;
		JVector2 prevPivot = { 0, 0 };
		JVector2 prevSize = { -1, -1 };
	};
	template<class Type>
	class GUIDropData
	{
	public:
		Type Data;
		size_t DataSize;
	};


	class GUIDropSourceBase
	{
	public:
		virtual ~GUIDropSourceBase() {}
	public:
		std::function<void()> StartDragFunc;
		std::function<void()> DraggingFunc;
		std::function<void()> DropFunc;
		
	};

	template<class Type, size_t typeSize = sizeof(Type)>
	class GUIDropSource : public GUIDropSourceBase
	{
	public:
		virtual ~GUIDropSource() {}
		Type    Data;
		size_t  DataSize = 0;
		size_t  TypeSize = typeSize;
	};


	class GUIDropItem
	{
		friend LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
		friend class GraphicsIF;
	public:


		template<class Type, size_t typeSize = sizeof(Type)>
		std::unique_ptr<GUIDropData<Type>> GetDropSource(size_t i) const
		{
			if (DropSources.size() <= i) return nullptr;
			auto dropSource = DropSources[i].get();
			GUIDropSource<Type, typeSize>* dropSourceT = dynamic_cast<GUIDropSource<Type, typeSize>*>(dropSource);
			if (dropSourceT)
			{
				GUIDropData<Type> data;
				data.Data     = dropSourceT->Data;
				data.DataSize = dropSourceT->DataSize;
				return std::make_unique<GUIDropData<Type>>(data);
			}
			else return nullptr;
		}

		

		size_t GetDropSourceCount() const {
			return DropSources.size();
		}
	private:
		std::vector<std::unique_ptr<GUIDropSourceBase>> DropSources;
	};
    struct GUIIO
    {
        GUIIOFlags   IOFlags = GUIIOFlag_None;
		std::wstring FormDllPath  = TT("JGEditor.dll");
        std::wstring ResourcePath = TT("../Source/Resource/GUI/");
        std::wstring DefaultFont  = TT("../Source/Resource/Font/EngineFont.fnt");
        uint64_t    GarbageProcessCountPerTick   = 50;
        float       GarbageCollectionTick        = 0.5f;


		const int   FrameCount = 3;
        JVector2 MousePos;
        JVector2 PrevMousePos;
        float MouseDoubleClickTime = 0.3f;
        float MouseDoubleClickMaxDist = 6.0f;


        float KeyDownDuration[256];
        float KeyRepeatDelay = 0.016f;



        GUIIO()
        {
            for (int i = 0; i < 256; ++i)
            {
                KeyDownDuration[i] = 0.0f;
            }
        }
    };



	struct GUIStyle
	{
		/*
		Zpos 적용 시키기
		*/
		float    Alpha = 1.0f;
		float    WindowBorderThick = 1.0f;
		float    TitleBarHeight = 20;


		float    DefaultTextSize = 18;
		float    TextLineSpacing = 5.0f;
		float    TextTabSize = 16.0f;

		/* FieldHeight
		* 
		
		
		*/
		float FieldHeight = 20.0f;
		float FieldWidth  = 160.0f;

		JVector2 WindowPadding = { 8,8 };
		JVector2 WindowMinSize = { 32, 32 };
		JVector2 WindowResizeGripSize = { 25, 25 };
		JVector2 ButtonSize = { 50, 20 };


		std::wstring GUIImages[GUIStyle_Resource_Count];
		JColor GUIColors[GUIStyle_Count];
		GUIStyle(GUIIO& io)
		{
			GUIColors[GUIStyle_WindowBackGround] = JColor(0.17f, 0.17f, 0.2f, Alpha);
			GUIColors[GUIStyle_WindowBorder] = JColor(1.0f, 1.0f, 1.0f, Alpha);
			GUIColors[GUIStyle_WindowResizeGrip] = JColor(0.14f, 0.67f, 0.95f, 0.75f);
			GUIColors[GUIStyle_TitleBar] = JColor(0.37f, 0.37f, 0.66f, Alpha - 0.2f);
			GUIColors[GUIStyle_TitleText] = JColor(1.0f, 1.0f, 1.0f, 1.0f);
			GUIColors[GUIStyle_ButtonNormal] = JColor(0.25f, 0.25f, 0.25f, Alpha);
			GUIColors[GUIStyle_ButtonHightlight] = JColor(0.5f, 0.5f, 0.5f, Alpha);
			GUIColors[GUIStyle_ButtonPressed] = JColor(0.1f, 0.1f, 0.1f, Alpha);
			GUIColors[GUIStyle_ButtonSelected] = JColor(0.25f, 0.25f, 0.25f, Alpha);
			GUIColors[GUIStyle_ButtonDisable] = JColor(0.0f, 0.0f, 0.0f, 1.0f);

			GUIColors[GUIStyle_SliderHandle]     = JColor(1.0f, 0.1f, 0.1f, Alpha);
			GUIColors[GUIStyle_SliderBackGround] = JColor(0.5f, 0.5f, 0.5f, Alpha);
			GUIColors[GUIStyle_SliderFillArea]   = JColor(1.0f, 1.0f, 1.0f, Alpha);

			GUIColors[GUIStyle_WindowResizeGripHightlight] = JColor(0.95f, 0.67f, 0.14f, 0.75f);


			GUIImages[GUIStyle_Resource_CloseBt] = io.ResourcePath + TT("CloseBt.png");
			GUIImages[GUIStyle_Resource_MaxBt]   = io.ResourcePath + TT("MaxBt.png");
			GUIImages[GUIStyle_Resource_HideBt]  = io.ResourcePath + TT("HideBt.png");

			GUIImages[GUIStyle_Resource_Check]          = io.ResourcePath + TT("Check.png");
			GUIImages[GUIStyle_Resource_RightFillArrow] = io.ResourcePath + TT("RightFillArrow.png");
			GUIImages[GUIStyle_Resource_RightArrow]     = io.ResourcePath + TT("RightArrow.png");
			GUIImages[GUIStyle_Resource_LeftFillArrow]  = io.ResourcePath + TT("LeftFillArrow.png");
			GUIImages[GUIStyle_Resource_LeftArrow]      = io.ResourcePath + TT("LeftArrow.png");
			GUIImages[GUIStyle_Resource_UpFillArrow]    = io.ResourcePath + TT("UpFillArrow.png");
			GUIImages[GUIStyle_Resource_UpArrow]        = io.ResourcePath + TT("UpArrow.png");
			GUIImages[GUIStyle_Resource_DownFillArrow]  = io.ResourcePath + TT("DownFillArrow.png");
			GUIImages[GUIStyle_Resource_DownArrow]      = io.ResourcePath + TT("DownArrow.png");
		}
	};


	struct JFontCharInfo
	{
		int id = 0;
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		int xoffset = 0;
		int yoffset = 0;
		int xadvance = 0;
		int pageID = 0;
		int chnl = 0;
	};
	struct JFontFileInfo
	{
		std::wstring face;
		int default_font_size = 0;
		bool bold = false;
		bool italic = false;
		std::wstring charset;
		bool unicode = false;
		int stretchH = 0;
		bool smooth = false;
		int super_sampling = 0;
		int padding[4];
		int spacing[2];
		int outline = 0;
		int lineHeight = 32;
		int base = 27;
		float scaleW = 0.0f;
		float scaleH = 0.f;
		int packed = 0;
		int alphaChnl = 0;
		int redChnl = 0;
		int greenChnl = 0;
		int blueChnl = 0;
		int pageCount = 0;
		std::map<int, std::wstring> page;
		int charCount = 0;
	};

	class GUI_API JFont
	{
		friend JFontManager;
	private:
		std::wstring   m_Path;
		JFontFileInfo  m_FileInfo;
		std::unordered_map<int, JFontCharInfo> m_CharMap;
	public:
		const JFontFileInfo& GetFileInfo() const {
			return m_FileInfo;
		}
		const std::wstring& GetPath() const {
			return m_Path;
		}
		const JFontCharInfo& GetCharInfo(wchar_t c)
		{
			auto& iter = m_CharMap.find((int)c);
			if (iter == m_CharMap.end())
			{
				return m_CharMap[0];
			}
			return iter->second;
		}


	private:
		static void Next(std::ifstream& fin) {
			char c = 0;
			while (c != '=')
				fin >> c;
		}
		static bool Load(const std::wstring& path, JFont& out)
		{
			std::ifstream fin;
			fin.open(path);
			if (!fin.is_open()) return false;

			std::string bf;
			char cf = 0;
			out.m_Path = path;
			Next(fin); fin >> bf; out.m_FileInfo.face = s2ws(bf);
			// 폰트 기본 사이즈
			Next(fin); fin >> out.m_FileInfo.default_font_size;
			// 폰트 Bold
			Next(fin); fin >> out.m_FileInfo.bold;
			// 폰트 Italic
			Next(fin); fin >> out.m_FileInfo.italic;
			// 폰트 charset
			Next(fin); fin >> bf; out.m_FileInfo.charset = s2ws(bf);
			// 폰트 유니코드 여부
			Next(fin); fin >> out.m_FileInfo.unicode;
			// 
			Next(fin); fin >> out.m_FileInfo.stretchH;
			//
			Next(fin); fin >> out.m_FileInfo.smooth;
			//
			Next(fin); fin >> out.m_FileInfo.super_sampling;
			// 
			Next(fin);
			fin >> cf; out.m_FileInfo.padding[0] = std::stoi(&cf);
			fin >> cf;
			fin >> cf; out.m_FileInfo.padding[1] = std::stoi(&cf);
			fin >> cf;
			fin >> cf; out.m_FileInfo.padding[2] = std::stoi(&cf);
			fin >> cf;
			fin >> cf; out.m_FileInfo.padding[3] = std::stoi(&cf);

			Next(fin);
			fin >> cf; out.m_FileInfo.spacing[0] = std::stoi(&cf);
			fin >> cf;
			fin >> cf; out.m_FileInfo.spacing[1] = std::stoi(&cf);
			//
			Next(fin); fin >> out.m_FileInfo.outline;
			//				 
			Next(fin); fin >> out.m_FileInfo.lineHeight;
			//			
			Next(fin); fin >> out.m_FileInfo.base;
			//				
			Next(fin); fin >> out.m_FileInfo.scaleW;
			//			
			Next(fin); fin >> out.m_FileInfo.scaleH;
			//			
			Next(fin); fin >> out.m_FileInfo.pageCount;
			//		
			Next(fin); fin >> out.m_FileInfo.packed;
			//		
			Next(fin); fin >> out.m_FileInfo.alphaChnl;
			//		
			Next(fin); fin >> out.m_FileInfo.redChnl;
			//			
			Next(fin); fin >> out.m_FileInfo.greenChnl;
			//		
			Next(fin); fin >> out.m_FileInfo.blueChnl;

			for (int i = 0; i < out.m_FileInfo.pageCount; ++i)
			{
				std::wstring wbf;
				int pageID = 0;
				Next(fin); fin >> pageID; Next(fin); fin >> bf; wbf = s2ws(bf);
				wbf = wbf.substr(1, wbf.size() - 2);
				std::filesystem::path p(path);
				auto filename = p.filename().string();
				wbf = path.substr(0, path.size() - filename.size()) + wbf;
				out.m_FileInfo.page[pageID] = wbf;
			}

			Next(fin); fin >> out.m_FileInfo.charCount;


			for (int i = 0; i < out.m_FileInfo.charCount; ++i)
			{
				JFontCharInfo charInfo;
				Next(fin); fin >> charInfo.id;
				Next(fin); fin >> charInfo.x;
				Next(fin); fin >> charInfo.y;
				Next(fin); fin >> charInfo.width;
				Next(fin); fin >> charInfo.height;
				Next(fin); fin >> charInfo.xoffset;
				Next(fin); fin >> charInfo.yoffset;
				Next(fin); fin >> charInfo.xadvance;
				Next(fin); fin >> charInfo.pageID;
				Next(fin); fin >> charInfo.chnl;


				out.m_CharMap[charInfo.id] = charInfo;
			}
			fin.close();
			return true;
		}
	};



	class GUI_API JFontManager
	{
	public:
		JFont* Load(const std::wstring& path) {
			{
				std::shared_lock<std::shared_mutex> lock(m_FontMutex);
				if (m_FontPool.find(path) != m_FontPool.end())
				{
					return m_FontPool[path].get();
				}
			}

			auto font = std::make_unique<JFont>();
			if (!JFont::Load(path, *font))
				return nullptr;
			auto result = font.get();
			{
				std::lock_guard<std::shared_mutex> lock(m_FontMutex);
				m_FontPool[path] = move(font);
			}
			return result;
		}

	private:
		std::unordered_map<std::wstring, std::unique_ptr<JFont>> m_FontPool;
		std::shared_mutex m_FontMutex;
	};
}
