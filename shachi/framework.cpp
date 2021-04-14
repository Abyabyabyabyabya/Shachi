#include "framework.hpp"
#include <functional>
#include <chrono>
#include <tchar.h>
#include "project_settings.hpp"

#include "level.hpp"

namespace
{
  /* 関数 */
    HWND launchWindow(HINSTANCE);
    LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
    void mainLoop(const std::function<void()>& TickEvent);

  /* 定数 */
    constexpr DWORD kWindowStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    constexpr DWORD kWindowStyleEx = WS_EX_OVERLAPPEDWINDOW;
    constexpr std::chrono::microseconds kTimePerFrameUs {16666LL};

  /* 変数 */
    int gExitCode {0};
} // unnamed namespace

int Framework::run(HINSTANCE HInstance)
{
    launchWindow(HInstance);

    Level lev;
    if( !lev.initialize() ) return -1;
    mainLoop(std::bind(&Level::update, &lev));
    lev.finalize();

    return gExitCode;
}

void Framework::quit(int ExitCode)
{
    PostQuitMessage(ExitCode);
    gExitCode = ExitCode;
}

namespace {
HWND launchWindow(HINSTANCE HInstance)
{
    // ウィンドウ定義を登録
    // 1度の呼び出しでもよいが、呼び出し元のrunが1度しか呼ばれないので問題ない
    WNDCLASSEX wnd {
        sizeof(WNDCLASSEX),                       // cbSize,
        CS_VREDRAW,                               // style
        wndProc,                                  // lpfnWndProc
        0,                                        // cbClsExtra
        0,                                        // cbWndExtra
        GetModuleHandle(nullptr),                 // hInstance
        0,                                        // hIcon
        LoadCursor(nullptr, IDC_ARROW),           // hCursor
        reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), // hbrBackground
        nullptr,                                  // lpszMenuName
        _T("shachi-project-wndcls"),              // lpszClassName
        0                                         // hIconSm
    };

    if(!RegisterClassEx(&wnd))
        return nullptr;


    // ウィンドウを作成
    // ウィンドウのサイズは 設定サイズ+枠 になる
    RECT valid_region {
        0L, 0L, ProjectSettings::kWindowWidth<long>, ProjectSettings::kWindowHeight<long>
    };
    if(!AdjustWindowRectEx(&valid_region, kWindowStyle, false, kWindowStyleEx))
        return nullptr;

    HWND hwnd = CreateWindowEx(
        kWindowStyleEx,                         // dwStyle
        _T("shachi-project-wndcls"),            // lpClassName
        _T("shachi-cu"),                        // lpWindowName
        kWindowStyle,                           // dwStyle
        CW_USEDEFAULT,                          // X
        CW_USEDEFAULT,                          // Y
        valid_region.right - valid_region.left, // nWidth
        valid_region.bottom - valid_region.top, // nHeight
        nullptr,                                // hWndParent
        nullptr,                                // hMenu
        HInstance,                              // hInstance
        nullptr                                 // lpParam
    );

    if(hwnd == nullptr)
        return nullptr;


    // 作成したウィンドウを表示
    // 第2引数はWinMainの引数を渡す方が正しいかも (今のところ特に問題がないのでこのまま)
    ShowWindow(hwnd, SW_NORMAL);


    return hwnd;
}

LRESULT CALLBACK wndProc(HWND HWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch(Message) {
    case WM_KEYDOWN :
        // Escキーの押下で終了
        if(WParam == VK_ESCAPE)
            Framework::quit(0);
        return 0;

    case WM_DESTROY :
        Framework::quit(0);
        return 0;
    }

    return DefWindowProc(HWnd, Message, WParam, LParam);
}

void mainLoop(const std::function<void()>& Tick)
{
    using namespace std::chrono;
    
    auto last  = high_resolution_clock::now() - kTimePerFrameUs;  // 最終計測時点 (1フレーム目を即実行するため、kTimeperFrameUS で減算)
    auto over = last - last;                                      // over = elapsed % TimePerFrame

    while(true) {
        // メッセージ処理
        MSG msg{};
        bool is_quit {false};
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            is_quit |= (msg.message == WM_QUIT); // 終了メッセージを受け取ったら、プログラムも終了する
        }
        if(is_quit) break;

        // フレームレート管理
        auto now = high_resolution_clock::now();
        auto elapsed_us = duration_cast<microseconds>(now - last) + over;
        if(elapsed_us >= kTimePerFrameUs) {
            last = now;
            over = elapsed_us % kTimePerFrameUs;

            Tick();
        }
    }
}
}
// EOF
