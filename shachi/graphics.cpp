#include "graphics.hpp"
#include "project_settings.hpp"
#include "show_error.hpp"

namespace
{
    constexpr D3D_FEATURE_LEVEL kFeatureLevels[] {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

#ifdef _DEBUG
    constexpr UINT kDeviceCreateFlag = D3D11_CREATE_DEVICE_DEBUG;
#else
    constexpr UINT kDeviceCreateFlag = 0U;
#endif
} // unnamed namespace

std::unique_ptr<Renderer> Graphics::createRenderer()
{
    return nullptr;
}

std::unique_ptr<Layer> Graphics::createLayer()
{
    D3D11_TEXTURE2D_DESC tex_desc{};
    

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT result = device_->CreateTexture2D(nullptr, nullptr, &texture);
    if(FAILED(result)) return nullptr;


    return std::make_unique<Layer>(texture.Get());
}

bool Graphics::initialize(HWND Window)
{
    DXGI_SWAP_CHAIN_DESC sc_desc{};
    sc_desc.BufferDesc.Width = ProjectSettings::kWindowWidth<UINT>;
    sc_desc.BufferDesc.Height = ProjectSettings::kWindowHeight<UINT>;
    sc_desc.BufferDesc.RefreshRate.Numerator = 60U;
    sc_desc.BufferDesc.RefreshRate.Denominator = 1U;
    sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    /*BufferDesc.ScanlineOrdering*/
    /*BufferDesc.Scaling*/
    sc_desc.SampleDesc.Count = 1U;
    sc_desc.SampleDesc.Quality = 0U;
    sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc_desc.BufferCount = 1;
    sc_desc.OutputWindow = Window;
    sc_desc.Windowed = ProjectSettings::kWindowed;
    /*.SwapEffect*/
    sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


    // デバイスリソースを作成
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        kDeviceCreateFlag,
        kFeatureLevels,
        sizeof(kFeatureLevels) / sizeof(D3D_FEATURE_LEVEL),
        D3D11_SDK_VERSION,
        &sc_desc,
        &swap_chain_,
        &device_,
        &feature_level_,
        &immediate_context_
    );

    if(FAILED(result)) {
        showErrorDialog(TEXT("Failed to create device resources"));
        return false;
    }


    // ベースレイヤーを作成
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
    result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
    if(FAILED(result)) {
        showErrorDialog(TEXT("Failed to get backbuffer from IDXGISwapChain"));
        return false;
    }
    base_layer_ = std::make_unique<Layer>(backbuffer.Get());

    return true;
}

void Graphics::finalize()
{

}

void Graphics::update()
{
    swap_chain_->Present(0, 0);
}
// EOF
