#ifndef INCLUDED_SHACHI_SHOW_ERROR_HPP_
#define INCLUDED_SHACHI_SHOW_ERROR_HPP_

#include <Windows.h>
#include <tchar.h>

void showErrorDialog(_In_opt_ LPCSTR Message)
{
    MessageBoxA(nullptr, Message, "Error", MB_OK);
}

void showErrorDialog(_In_opt_ LPCWSTR Message)
{
    MessageBoxW(nullptr, Message, L"Error", MB_OK);
}

#endif // !INCLUDED_SHACHI_SHOW_ERROR_HPP_
// EOF
