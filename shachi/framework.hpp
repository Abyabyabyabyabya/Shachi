#ifndef INCLUDED_SHACHI_FRAMEWORK_HPP_
#define INCLUDED_SHACHI_FRAMEWORK_HPP_

#include <Windows.h>

///
/// @brief  �v���W�F�N�g�t���[�����[�N
///          - �E�B���h�E�̋N��
///          - ���C�����[�v (�E�B���h�E���b�Z�[�W�����AFPS�Ǘ�)
///
class Framework
{
public :
    static int run(HINSTANCE);
    static void quit(int ExitCode =0);
};

#endif // !INCLUDED_SHACHI_FRAMEWORK_HPP_
// EOF
