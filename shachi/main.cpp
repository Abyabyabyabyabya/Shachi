#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include <Windows.h>
#include "framework.hpp"

 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#ifdef _DEBUG
    //_CrtSetBreakAlloc();
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    return Framework::run(hInstance);
}
// EOF
