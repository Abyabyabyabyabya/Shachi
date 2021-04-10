#ifndef INCLUDED_SHACHI_LEVEL_HPP_
#define INCLUDED_SHACHI_LEVEL_HPP_

#include "shachi.hpp"

class Level
{
public :
    virtual ~Level() = default;

    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual void update() = 0; // and draw
};

#endif // !INCLUDED_SHACHI_LEVEL_HPP_
// EOF
