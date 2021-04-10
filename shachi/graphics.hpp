#ifndef INCLUDED_SHACHI_GRAPHICS_HPP_
#define INCLUDED_SHACHI_GRAPHICS_HPP_

#include <cassert>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

class Graphics
{
public :
    static void create(HWND hWnd);
    static void destroy();

    static void beginRender();
    static void endRender();

    static ID3D11Device* d3dDevice() noexcept { assert(!!instance_); return instance_->device_.Get(); }
    static ID3D11DeviceContext* d3dImmediateContext() noexcept { assert(!!instance_); return instance_->immediate_context_.Get(); }
    static ID3D11RenderTargetView* backbuffer() noexcept { assert(!!instance_); return instance_->backbuffer_.Get(); }

private :
    Graphics() = default;

    static std::unique_ptr<Graphics> instance_;

    D3D_FEATURE_LEVEL feature_level_;
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backbuffer_;
    bool beggined_;
};

#endif // !INCLUDED_SHACHI_GRAPHICS_HPP_
// EOF
