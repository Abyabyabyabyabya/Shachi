#ifndef INCLUDED_SHACHI_LAYER_HPP_
#define INCLUDED_SHACHI_LAYER_HPP_

#include <d3d11.h>
#include <wrl.h>

///
/// @brief  レイヤー
///         描画先としてレンダラに渡すクラス
/// 
class Layer
{
public :
    Layer(ID3D11Texture2D* Texture);

    ID3D11Texture2D* getNativeTexture() const { return texture_.Get(); }
private :
    int width_;
    int height_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
};

#endif // !INCLUDED_SHACHI_LAYER_HPP_
// EOF
