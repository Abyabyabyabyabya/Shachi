#ifndef INCLUDED_SHACHI_GRAPHICS_HPP_
#define INCLUDED_SHACHI_GRAPHICS_HPP_

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "renderer.hpp"
#include "layer.hpp"

/// 
/// @brief  描画システムレイヤ
/// 
class Graphics
{
public :
    std::unique_ptr<Renderer> createRenderer();
    std::unique_ptr<Layer>    createLayer();

    bool initialize(HWND TargetWindow);
    void finalize();
    void update();

    Layer* baseLayer() const noexcept { return base_layer_.get(); }
private :
    template <class T> using handler_type = Microsoft::WRL::ComPtr<T>;

    D3D_FEATURE_LEVEL feature_level_;
    handler_type<ID3D11Device> device_;
    handler_type<ID3D11DeviceContext> immediate_context_;
    handler_type<IDXGISwapChain> swap_chain_;
    std::unique_ptr<Layer> base_layer_;
};

#endif // !INCLUDED_SHACHI_GRAPHICS_HPP_
// EOF
