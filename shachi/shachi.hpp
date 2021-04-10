#ifndef INCLUDED_SHACHI_SHACHI_HPP_
#define INCLUDED_SHACHI_SHACHI_HPP_

#include <memory>
#include <Windows.h>

class Level;

class Shachi
{
public :
    static int run(HINSTANCE hInstance);
    static void quit(int code=0);

    static void changeLevel(std::unique_ptr<Level> Next);
};

#endif // !INCLUDED_SHACHI_SHACHI_HPP_
// EOF
