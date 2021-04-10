#include "shachi.hpp"
#include <chrono>
#include "graphics.hpp"
#include "level.hpp"

#include <string>
#include "level_sandbox.hpp"

namespace {

struct ShachiState {
    std::unique_ptr<Level> current_level;  
    std::unique_ptr<Level> next_level;
};

LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);


constexpr std::chrono::microseconds kUsPerFrame {16666LL};
constexpr long kWindowWidth  = 1280L;
constexpr long kWindowHeight = 720L;
ShachiState gState{};
bool gIsContinue {true};
int  gExitCode {0};

} // unnamed namespace

int Shachi::run(HINSTANCE hInstance) {
    // create & show Window
    WNDCLASSEX wnd_class
    {
        sizeof(WNDCLASSEX),
        CS_VREDRAW,
        wndProc,
        0,
        0,
        hInstance,
        nullptr,
        LoadCursor(nullptr, IDC_ARROW),
        reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
        nullptr,
        L"shachi-are",
        nullptr
    };
    if(!RegisterClassEx(&wnd_class)) return -1;

    RECT rect {0L, 0L, kWindowWidth, kWindowHeight };
    if(!AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE, WS_EX_OVERLAPPEDWINDOW)) return -1;
    auto hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"shachi-are", L"shachi", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, rect.right-rect.left, rect.bottom-rect.top, nullptr, nullptr, hInstance, nullptr);
    if(hWnd == nullptr) return -1;

    ShowWindow(hWnd, SW_NORMAL);

    // initialize game
    Graphics::create(hWnd);
    gState.current_level = std::make_unique<Sandbox>();
    gState.current_level->initialize();

    // main loop
    using namespace std::chrono;
    auto lst = high_resolution_clock::now();
    auto dlt = lst - lst;
    while(gIsContinue) {
        MSG msg{};
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(gState.next_level) {
            gState.next_level->initialize();
            gState.current_level->finalize();
            gState.current_level = std::move(gState.next_level);
        }

        auto now = high_resolution_clock::now();
        auto elapsed = duration_cast<microseconds>(now - lst) + dlt;
        if(elapsed >= kUsPerFrame) {
            lst = now;
            dlt = elapsed % kUsPerFrame;

            gState.current_level->update();

            //---------------------
            SetWindowText(hWnd, std::to_wstring((elapsed+dlt).count()).c_str());
        }
    }

    // finalize game
    gState.current_level->finalize();
    Graphics::destroy();

    return gExitCode;
}

void Shachi::quit(int ExitCode) {
    gIsContinue = false;
    gExitCode = ExitCode;
}

void Shachi::changeLevel(std::unique_ptr<Level> NextLevel) {
    gState.next_level = std::move(NextLevel);
}

namespace { LRESULT CALLBACK wndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
    case WM_KEYDOWN :
    case WM_SYSKEYDOWN :
        if(wParam == VK_ESCAPE) {
            PostQuitMessage(0);
            gIsContinue = false;
        }
        break;

    case WM_DESTROY :
        PostQuitMessage( 0 );
        gIsContinue = false;
        return 0;
    }

    return DefWindowProc(hWnd, Message, wParam, lParam);
}}
// EOF
