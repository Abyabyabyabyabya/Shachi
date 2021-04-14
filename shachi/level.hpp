#ifndef INCLUDED_SHACHI_LEVEL_HPP_D
#define INCLUDED_SHACHI_LEVEL_HPP_D

///
/// @brief  レベルクラス
///         ver2.0時点では多態性を必要としないので、抽象クラスとしていない
class Level
{
public :
    bool initialize();
    void finalize();
    void update();
};
#endif // !INCLUDED_SHACHI_LEVEL_HPP_D
// EOF
