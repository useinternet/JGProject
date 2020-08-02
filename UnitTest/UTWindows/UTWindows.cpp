// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "UTWindows.h"


#include "GraphicsIF.h"
#include "GraphicsEngine.h"
#include <filesystem>
#include <fstream>
#include <random>
using namespace std;
namespace fs = std::filesystem;
#define MAX_LOADSTRING 100
#pragma comment (lib, "GraphicsEngine.lib")
void LoadMesh(vector<StaticVertex>& _v, vector<uint32_t>& _i, string path)
{

    fs::path file_path(path);
    auto file_format = file_path.extension();

    if (file_format != ".mesh")
        return;


    ifstream fin;
    fin.open(path, ios::binary);

    // 메시 데이터 갯수
    uint32_t meshCount = 0;
    fin.read(
        reinterpret_cast<char*>(&meshCount),
        sizeof(uint32_t));


    std::vector<std::string> MeshNames(meshCount);

    // 스켈레톤 메시 인가 아닌가.
    bool is_skeletal = false;
    fin.read(
        reinterpret_cast<char*>(&is_skeletal),
        sizeof(bool));

    for (uint32_t i = 0; i < meshCount; ++i)
    {
        // 메시 이름 길이
        uint32_t name_len = 0;
        fin.read(
            reinterpret_cast<char*>(&name_len),
            sizeof(uint32_t));

        // 메시 이름
        char* mesh_name_Cstr = new char[name_len];
        fin.read(
            reinterpret_cast<char*>(mesh_name_Cstr),
            sizeof(char) * name_len);
        std::string meshname = mesh_name_Cstr;
        MeshNames[i] = meshname;
        delete[] mesh_name_Cstr;


        // Vertex & Index 갯수
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        fin.read(
            reinterpret_cast<char*>(&vertexCount),
            sizeof(uint32_t));
        fin.read(
            reinterpret_cast<char*>(&indexCount),
            sizeof(uint32_t));

        if (is_skeletal)
        {
            assert(false && "current is not supported skeletalmesh");
        }
        else // 스태틱 메시 일경우
        {
            std::vector<StaticVertex> vertices;
            std::vector<uint32_t> indices;

            vertices.resize(vertexCount);
            indices.resize(indexCount);

            fin.read(
                reinterpret_cast<char*>(vertices.data()),
                sizeof(StaticVertex) * vertexCount);
            fin.read(
                reinterpret_cast<char*>(indices.data()),
                sizeof(uint32_t) * indexCount);


            _v.insert(_v.end(), vertices.begin(), vertices.end());
            _i.insert(_i.end(), indices.begin(), indices.end());
        }
    }
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{


    return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND g_hWnd;
void Init();
void Update();
void Destroy();
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = L"Test";
    wcex.hIconSm = LoadIcon(0, IDI_APPLICATION);

    RegisterClassExW(&wcex);
    long flag = WS_POPUP | WS_VISIBLE;


    g_hWnd = CreateWindowW(L"Test", L"Test", flag,
        0, 0, 800, 600, 0, nullptr, hInst, nullptr);



    ShowWindow(g_hWnd, SW_NORMAL);
    UpdateWindow(g_hWnd);
    Init();

    bool is_running = true;
    MSG msg = {};
    while (is_running)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                int num = 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Update();

        if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000))
        {
            Destroy();
            is_running = false;
        }
     
    }

    return (int) msg.wParam;
}

using namespace GE;
class UnitTest* unitTest = nullptr;


class UnitTest
{
private:
    ScreenRef m_Screen;
    SceneCamera m_Cam;
    TextureRef t;

    std::vector<SDObject> m_Instances;
    std::vector<PaperVertex> v;
    std::vector<uint32_t> i;



    SceneRef m_Scene;
    std::vector<SceneObjectRef> m_Objects;
    std::vector<GE::SceneObject*> m_ObjectPtrs;
    std::vector<TextureRef> m_Textures;

public:
    void Init()
    {
        GraphicsIF::RegisterInVaildTexture(TT("../Source/Resource/Texture/Engine/InvaildTexture.png"));

        for (int i = 1; i <= 10; ++i)
        {
            wstring path = TT("../Source/Resource/Test/icon_weather");
            path += TT("_") + to_wstring(i) + TT(".png");
            m_Textures.push_back(move(GraphicsIF::LoadTexture(path)));
        }

        MeshGenerator::Quad(100.0f, 100.0f, 0.0f, v, i);
        // 스크린 생성
        m_Screen = GraphicsIF::RequestScreen(g_hWnd, 800, 600);

        m_Cam.SetLens(JG_PI * 0.25f, 800, 600, 1000.0f, 0.1f, true);
        m_Cam.SetPosition({ 0.0f,0.0f,-1000.0f });

   
      //  LoadMesh(v, i, "../UnitTest/Resource/cerberus.mesh");
        m_Instances.resize(100);

        // 머터리얼 생성
        MaterialProperty p;
        p.rendererMode = RendererMode::GUI;
        p.blendMode = BlendMode::Transparent;
        MaterialRef matRef = GraphicsIF::RequestMaterial(L"TestMaterial", p);
        matRef->DefineVarAsFloat4("TestColor", { 1.0f,0.0f,0.0f,1.0f });
        matRef->DefineTexture("Image", nullptr);
        matRef->SetCode(R"(
pout.Albedo = Image_Texture.Sample(g_AnisotropicSampler, pin.tex) * TestColor_Data;

)");
        GraphicsIF::PreCompileMaterial(matRef.Get());


        m_Scene = GraphicsIF::RequestScene();

        m_Scene->BindCamera(&m_Cam);
    }
    bool is_prev_key = false;
    bool is_key = false;

    JColor colorMap[3];
    void Update()
    {
        colorMap[0] = JColor::White();
        colorMap[1] = JColor(1.0f, 0.0f, 0.0f, 0.2f);
        if (GetAsyncKeyState(KeyCode::A) & 0x8000)
        {
            is_key = true;
            
        }
        else
        {
            is_key = false;
        }
        static float z = -400;
        static int index = 0;
        if (!is_key && is_prev_key)
        {
      
            SceneObjectRef o = GraphicsIF::RequestSceneObject();
            o->BindMesh(v.data(), v.size(), i.data(), i.size());
            z += 100;
          //  z -= 0.0001f;
            m_Instances[m_ObjectPtrs.size()].world = JMatrix::Transpose(JMatrix::Translation({ z,0.0f, 0.0f }));
            o->BindInstanceData(&m_Instances[m_ObjectPtrs.size()], 1);
      
            o->SetMeshType(GE::MeshType::Static);
            o->BindMaterial(GraphicsIF::GetMaterial(L"TestMaterial").Get());
            o->GetMaterial()->BindTexture("Image", m_Textures[index++].Get());
            random_device rd;
            float r = (float)(rd() % 10000) / 10000;
            float g = (float)(rd() % 10000) / 10000;
            float b = (float)(rd() % 10000) / 10000;
            float a = (float)(rd() % 10000) / 10000;
            if (m_ObjectPtrs.size() == 0 || m_ObjectPtrs.size() == 1)
            {
                o->GetMaterial()->SetVarAsFloat4("TestColor", colorMap[m_ObjectPtrs.size()]); 
            }
            else
            {
                o->GetMaterial()->SetVarAsFloat4("TestColor", { r,g,b,a });
            }
 
            m_ObjectPtrs.push_back(o.Get());
            m_Objects.push_back(move(o));
        }

        if (m_Scene.Get())

        {
            if (!m_ObjectPtrs.empty())
            {
                auto test = m_ObjectPtrs;
                m_Scene->PushObjects(test);
            }
            m_Screen->BindScene(m_Scene.Get());
            GraphicsIF::BeginDraw();
            GraphicsIF::DrawCall(m_Scene);
            GraphicsIF::EndDraw();
        }
   
        is_prev_key = is_key;

    }
    void Destroy()
    {
        m_Screen.Reset();
    }
};

void Init()
{


    GraphicsIF::Create();


    unitTest = new UnitTest();
    unitTest->Init();

   
}

void Update()
{
    //DXDevice::BeginFrame();
    unitTest->Update();

    //DXDevice::EndFrame();

}

void Destroy()
{
    unitTest->Destroy();
    delete unitTest;
    GraphicsIF::Destroy();
}
