#ifndef INCLUDED_SHACHI_FRAMEWORK_HPP_
#define INCLUDED_SHACHI_FRAMEWORK_HPP_

#include <Windows.h>

///
/// @brief  プロジェクトフレームワーク
///          - ウィンドウの起動
///          - メインループ (ウィンドウメッセージ処理、FPS管理)
///
class Framework
{
public :
    static int run(HINSTANCE);
    static void quit(int ExitCode =0);
};

#endif // !INCLUDED_SHACHI_FRAMEWORK_HPP_
// EOF
