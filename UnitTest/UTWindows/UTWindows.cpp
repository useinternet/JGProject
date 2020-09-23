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
//void LoadMesh(vector<StaticVertex>& _v, vector<uint32_t>& _i, string path)
//{
//
//    fs::path file_path(path);
//    auto file_format = file_path.extension();
//
//    if (file_format != ".mesh")
//        return;
//
//
//    ifstream fin;
//    fin.open(path, ios::binary);
//
//    // 메시 데이터 갯수
//    uint32_t meshCount = 0;
//    fin.read(
//        reinterpret_cast<char*>(&meshCount),
//        sizeof(uint32_t));
//
//
//    std::vector<std::string> MeshNames(meshCount);
//
//    // 스켈레톤 메시 인가 아닌가.
//    bool is_skeletal = false;
//    fin.read(
//        reinterpret_cast<char*>(&is_skeletal),
//        sizeof(bool));
//
//    for (uint32_t i = 0; i < meshCount; ++i)
//    {
//        // 메시 이름 길이
//        uint32_t name_len = 0;
//        fin.read(
//            reinterpret_cast<char*>(&name_len),
//            sizeof(uint32_t));
//
//        // 메시 이름
//        char* mesh_name_Cstr = new char[name_len];
//        fin.read(
//            reinterpret_cast<char*>(mesh_name_Cstr),
//            sizeof(char) * name_len);
//        std::string meshname = mesh_name_Cstr;
//        MeshNames[i] = meshname;
//        delete[] mesh_name_Cstr;
//
//
//        // Vertex & Index 갯수
//        uint32_t vertexCount = 0;
//        uint32_t indexCount = 0;
//        fin.read(
//            reinterpret_cast<char*>(&vertexCount),
//            sizeof(uint32_t));
//        fin.read(
//            reinterpret_cast<char*>(&indexCount),
//            sizeof(uint32_t));
//
//        if (is_skeletal)
//        {
//            assert(false && "current is not supported skeletalmesh");
//        }
//        else // 스태틱 메시 일경우
//        {
//            std::vector<StaticVertex> vertices;
//            std::vector<uint32_t> indices;
//
//            vertices.resize(vertexCount);
//            indices.resize(indexCount);
//
//            fin.read(
//                reinterpret_cast<char*>(vertices.data()),
//                sizeof(StaticVertex) * vertexCount);
//            fin.read(
//                reinterpret_cast<char*>(indices.data()),
//                sizeof(uint32_t) * indexCount);
//
//
//            _v.insert(_v.end(), vertices.begin(), vertices.end());
//            _i.insert(_i.end(), indices.begin(), indices.end());
//        }
//    }
//}
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

public:
    void Init()
    {

    }

    void Update()
    {

    }
    void Destroy()
    {

    }
};

void Init()
{


   // GraphicsIF::Create();


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
    //GraphicsIF::Destroy();
}
