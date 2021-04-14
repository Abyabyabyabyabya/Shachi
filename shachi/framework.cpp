#include "framework.hpp"
#include <functional>
#include <chrono>
#include <tchar.h>
#include "project_settings.hpp"

#include "level.hpp"

namespace
{
  /* �֐� */
    HWND launchWindow(HINSTANCE);
    LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
    void mainLoop(const std::function<void()>& TickEvent);

  /* �萔 */
    constexpr DWORD kWindowStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    constexpr DWORD kWindowStyleEx = WS_EX_OVERLAPPEDWINDOW;
    constexpr std::chrono::microseconds kTimePerFrameUs {16666LL};

  /* �ϐ� */
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
    // �E�B���h�E��`��o�^
    // 1�x�̌Ăяo���ł��悢���A�Ăяo������run��1�x�����Ă΂�Ȃ��̂Ŗ��Ȃ�
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


    // �E�B���h�E���쐬
    // �E�B���h�E�̃T�C�Y�� �ݒ�T�C�Y+�g �ɂȂ�
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


    // �쐬�����E�B���h�E��\��
    // ��2������WinMain�̈�����n���������������� (���̂Ƃ�����ɖ�肪�Ȃ��̂ł��̂܂�)
    ShowWindow(hwnd, SW_NORMAL);


    return hwnd;
}

LRESULT CALLBACK wndProc(HWND HWnd, UINT Message, WPARAM WParam, LPARAM LParam)
{
    switch(Message) {
    case WM_KEYDOWN :
        // Esc�L�[�̉����ŏI��
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
    
    auto last  = high_resolution_clock::now() - kTimePerFrameUs;  // �ŏI�v�����_ (1�t���[���ڂ𑦎��s���邽�߁AkTimeperFrameUS �Ō��Z)
    auto over = last - last;                                      // over = elapsed % TimePerFrame

    while(true) {
        // ���b�Z�[�W����
        MSG msg{};
        bool is_quit {false};
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            is_quit |= (msg.message == WM_QUIT); // �I�����b�Z�[�W���󂯎������A�v���O�������I������
        }
        if(is_quit) break;

        // �t���[�����[�g�Ǘ�
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
