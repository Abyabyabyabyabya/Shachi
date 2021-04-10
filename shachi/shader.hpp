#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <d3d11.h>
#include <wrl.h>


template <class DataType>
class ConstantBuffer
{
    template <class Ty>
    struct ElementType
    {
        using type = Ty;
    };
    template <class Ty, size_t N>
    struct ElementType<Ty[N]>
    {
        using type = Ty;
    };
public :
    using element_type = typename ElementType<DataType>::type;

    ConstantBuffer() = default;
    ConstantBuffer(ID3D11Device* D3DDevice) 
    {
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = sizeof(DataType);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3DDevice->CreateBuffer(&desc, nullptr, &constant_buffer_);
    }
    ConstantBuffer(ID3D11Device* D3DDevice, const DataType& Data)
    {
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = sizeof(DataType);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA srd{};
        srd.pSysMem = &Data;
        D3DDevice->CreateBuffer(&desc, &srd, &constant_buffer_);
    }

    void update(ID3D11DeviceContext* Context, const DataType& Data)
    {
        D3D11_MAPPED_SUBRESOURCE mpd{};
        auto hr = Context->Map(constant_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mpd);
        assert(SUCCEEDED(hr));

        memcpy(mpd.pData, &Data, sizeof(DataType));

        Context->Unmap(constant_buffer_.Get(), 0);
    }
    
    template <class ElemType, size_t N, std::enable_if_t<std::is_array_v<ElemType>>* =nullptr>
    void update(ID3D11DeviceContext* Context, const ElemType(&ArrayData)[N])
    {
        static_assert(std::is_same_v<element_type, ElemType>);
        static_assert(N <= std::extent_v<DataType>);

        D3D11_MAPPED_SUBRESOURCE mpd{};
        auto hr = Context->Map(constant_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mpd);
        assert(SUCCEEDED(hr));

        memcpy(mpd.pData, ArrayData, sizeof(element_type) * N);

        Context->Unmap(constant_buffer_.Get(), 0);
    }

    ID3D11Buffer* buffer() const noexcept { return constant_buffer_.Get(); }

    bool isValic() const noexcept  { return !!constant_buffer_; }
    operator bool() const noexcept { return isValic(); }
private :
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_;
};

enum class ShaderType
{
    kVertexShader = 1,
    kPixelShader,
};

class ShaderImpl
{
public :
    virtual ~ShaderImpl() = default;
    virtual void setShaderToContext(ID3D11DeviceContext*, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>&, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11SamplerState>>&, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11Buffer>>&) = 0;
    virtual bool isValid() const noexcept = 0;
    virtual ShaderType type() const noexcept = 0;
};

class ShaderInfo
{
public :
    virtual ~ShaderInfo() = default;
    virtual std::unique_ptr<ShaderImpl> createShaderImpl(ID3D11Device* D3DDevice) const = 0;
};

class VertexShaderInfo : public ShaderInfo
{
public :
    template <class StrType, class InputDescType>
    VertexShaderInfo(StrType&& ShaderFilePath, InputDescType&& InputDescription) : shader_file_path_{std::forward<StrType>(ShaderFilePath)}, input_description_{std::forward<InputDescType>(InputDescription)} {}
    std::unique_ptr<ShaderImpl> createShaderImpl(ID3D11Device* D3DDevice) const override;
private :
    std::string shader_file_path_;
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_description_;
};

class PixelShaderInfo : public ShaderInfo
{
public :
    template <class StrType>
    PixelShaderInfo(StrType&& ShaderFilePath) : shader_file_path_{std::forward<StrType>(ShaderFilePath)} {}
    std::unique_ptr<ShaderImpl> createShaderImpl(ID3D11Device* D3DDevice) const override;

private :
    std::string shader_file_path_;
};

class Shader
{
public :
    Shader() = default;
    Shader(ID3D11Device* D3DDevice, const ShaderInfo& Info);

    void setShaderResource(ID3D11ShaderResourceView* ShaderResource, UINT Slot) { resource_views_[Slot] = ShaderResource; }
    void setSampler(ID3D11SamplerState* Sampler, UINT Slot) { samplers_[Slot] = Sampler; }
    void setConstantBuffer(ID3D11Buffer* ConstantBuffer, UINT Slot) { constants_[Slot] = ConstantBuffer; }

    void setShaderStageToContext(ID3D11DeviceContext*);

    bool isValid()  const noexcept { return shader_impl_->isValid(); }
    operator bool() const noexcept { return isValid(); }
private :
    std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resource_views_;
    std::map<UINT, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers_;
    std::map<UINT, Microsoft::WRL::ComPtr<ID3D11Buffer>> constants_;
    std::unique_ptr<ShaderImpl> shader_impl_;
};
// EOF
