#include "framework.hpp"
#include <chrono>

namespace
{
  /* �֐� */
    LRESULT wndProc(HWND, UINT, WPARAM, LPARAM);
    HWND createWindow(HINSTANCE);

  /* �萔 */
    constexpr std::chrono::microseconds kTimePerFrameUs {16666LL};

  /* �ϐ� */
    int gExitCode {0};
} // unnamed namespace

int Framework::run(HINSTANCE hInstance)
{
    using namespace std::chrono;

    auto last  = high_resolution_clock::now();  // �ŏI�v�����_
    auto over = last - last;                    // over = elapsed % TimePerFrame

    // ���C�����[�v
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

        // �Q�[���X�V
        auto now = high_resolution_clock::now();
        auto elapsed_us = duration_cast<microseconds>(now - last) + over;
        if(elapsed_us >= kTimePerFrameUs) {
            last = now;
            over = elapsed_us % kTimePerFrameUs;

            /* Update */
            // ��ƒn�_
            // �t���[�����[�N�̍쐬��
            // ���C�����[�v�͂ł����̂ŁA�E�B���h�E�̕\������ĊJ����
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
