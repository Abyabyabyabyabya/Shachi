#include "graphics.hpp"

#pragma comment (lib, "d3d11.lib")

namespace {

constexpr D3D_FEATURE_LEVEL kFeatureLevels[] {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_11_1,
};

} // namespace

std::unique_ptr<Graphics> Graphics::instance_;

void Graphics::create(HWND hWnd) {
    assert(!instance_ && "ASSERTION : Graphics::create - must be not-ready for singleton");

    instance_ = std::unique_ptr<Graphics>{new Graphics{}};

    DXGI_SWAP_CHAIN_DESC swapchain_desc{};
    swapchain_desc.BufferDesc.Width  = 1280;
    swapchain_desc.BufferDesc.Height = 720;
    swapchain_desc.BufferDesc.RefreshRate.Denominator = 60U;
    swapchain_desc.BufferDesc.RefreshRate.Numerator   = 1U;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapchain_desc.SampleDesc.Count = 1U;
    swapchain_desc.SampleDesc.Quality = 0U;
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.BufferCount = 2U;
    swapchain_desc.OutputWindow = hWnd;
    swapchain_desc.Windowed = true;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


    auto hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
#ifdef _DEBUG
        D3D11_CREATE_DEVICE_DEBUG,  
#else
        0,
#endif
        kFeatureLevels,
        2,
        D3D11_SDK_VERSION,
        &swapchain_desc,
        &instance_->swap_chain_,
        &instance_->device_,
        &instance_->feature_level_,
        &instance_->immediate_context_
    );

    assert(SUCCEEDED(hr) && "ASSERTION : Graphics::create - Failed to create D3DDeviceResources");

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    hr = instance_->swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture);
    assert(SUCCEEDED(hr) && "ASSERTION : Graphics::create - Failed to get texture from swapchain");
    hr = instance_->device_->CreateRenderTargetView(texture.Get(), nullptr, &instance_->backbuffer_);
    assert(SUCCEEDED(hr) && "ASSERTION : Graphics::create - Failed to create render-target-view");

    instance_->beggined_ = false;
}

void Graphics::destroy() {
    assert(!!instance_ && "ASSERTION : Graphics::destroy - must be ready for singleton");
}

void Graphics::beginRender() {
    assert(!!instance_ && "ASSERTION : Graphics::beginRender - must be ready for singleton");
    assert(!instance_->beggined_ && "ASSERTION : Graphics::beginRender - beginRender must be call after Graphics::endRender");

    instance_->immediate_context_->ClearState();
    instance_->beggined_ = true;
}

void Graphics::endRender() {
    instance_->swap_chain_->Present(0, 0);
    instance_->beggined_ = false;
}
// EOF
