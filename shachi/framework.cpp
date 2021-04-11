#include "framework.hpp"
#include <chrono>

namespace
{
  /* 関数 */
    LRESULT wndProc(HWND, UINT, WPARAM, LPARAM);
    HWND createWindow(HINSTANCE);

  /* 定数 */
    constexpr std::chrono::microseconds kTimePerFrameUs {16666LL};

  /* 変数 */
    int gExitCode {0};
} // unnamed namespace

int Framework::run(HINSTANCE hInstance)
{
    using namespace std::chrono;

    auto last  = high_resolution_clock::now();  // 最終計測時点
    auto over = last - last;                    // over = elapsed % TimePerFrame

    // メインループ
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

        // ゲーム更新
        auto now = high_resolution_clock::now();
        auto elapsed_us = duration_cast<microseconds>(now - last) + over;
        if(elapsed_us >= kTimePerFrameUs) {
            last = now;
            over = elapsed_us % kTimePerFrameUs;

            /* Update */
            // 作業地点
            // フレームワークの作成中
            // メインループはできたので、ウィンドウの表示から再開する
        }
    }

    return gExitCode;
}

void Framework::quit(int ExitCode)
{
    PostQuitMessage(ExitCode);
    gExitCode = ExitCode;
}
// EOF
