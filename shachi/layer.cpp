#include "layer.hpp"

Layer::Layer(ID3D11Texture2D* Texture) :
    texture_{Texture}
{
    if(!texture_) return;

    // テクスチャ情報を取得する
    D3D11_TEXTURE2D_DESC desc{};
    texture_->GetDesc(&desc);
    width_ = desc.Width;
    height_ = desc.Height;
}

// EOF
