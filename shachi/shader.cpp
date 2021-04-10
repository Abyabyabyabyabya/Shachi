#include "shader.hpp"
#include <cassert>
#include <fstream>
#include <vector>

namespace {

struct Heap
{
    Heap() = default;
    ~Heap()
    {
        delete bytecode;
    }
    Heap(const size_t Length) : length{Length}, bytecode{new char[Length]} {}
    Heap(Heap&& Rhs) noexcept : length{Rhs.length}, bytecode{Rhs.bytecode}
    {
        Rhs.bytecode = nullptr;
    }
    Heap& operator=(Heap&& Rhs) noexcept
    {
        length = Rhs.length;
        bytecode = Rhs.bytecode;
        Rhs.bytecode = nullptr;
    }
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    size_t length {0};
    char* bytecode {nullptr};
};

Heap loadBinary(const std::string& FilePath)
{
    std::fstream file{FilePath, std::ios::binary | std::ios::in};
    if(!file) return Heap{};

    file.seekg(0, std::ios::end);
    auto end = file.tellg();
    file.clear();
    file.seekg(0, std::ios::beg);
    auto beg = file.tellg();

    Heap data{static_cast<size_t>(end-beg)};
    file.read(data.bytecode, data.length);
    
    return data;
}

template <class DataType>
bool extractConsecutiveSlots(typename std::map<UINT, Microsoft::WRL::ComPtr<DataType>>::const_iterator& Begin, const typename std::map<UINT, Microsoft::WRL::ComPtr<DataType>>::const_iterator& End, UINT& StartSlot, std::vector<DataType*>& OutContainer)
{
    OutContainer.clear();
    if(Begin == End) return false;

    UINT last_slot = Begin->first - 1;
    StartSlot = Begin->first;
    while(Begin != End && Begin->first == last_slot+1) {
        OutContainer.emplace_back(Begin->second.Get());

        ++Begin;
        ++last_slot; // last_slot = Begin->first;
    }

    return OutContainer.size();
}

class EmptyShader : public ShaderImpl
{
public :
    void setShaderToContext(ID3D11DeviceContext*, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>&, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11SamplerState>>&, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11Buffer>>&) override {}
    bool isValid() const noexcept override { return false; }
    ShaderType type() const noexcept override { return static_cast<ShaderType>(0); }
};

class VertexShader : public ShaderImpl
{
public :
    VertexShader(ID3D11Device* D3DDevice, const std::string& FilePath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& InputElementDescs)
    {
        auto binary = loadBinary(FilePath);
        if(!binary.bytecode) return;

        auto hr = D3DDevice->CreateVertexShader(binary.bytecode, binary.length, nullptr, &shader_);
        if(FAILED(hr)) return;

        hr = D3DDevice->CreateInputLayout(InputElementDescs.data(), InputElementDescs.size(), binary.bytecode, binary.length, &input_layout_);
        if(FAILED(hr)) {
            shader_.Reset();
            return;
        }
    }

    void setShaderToContext(ID3D11DeviceContext* DC, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& Resources, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11SamplerState>>& Samplers, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11Buffer>>& Constants) override
    {
        DC->IASetInputLayout(input_layout_.Get());
        DC->VSSetShader(shader_.Get(), nullptr, 0);
        
        UINT slot;

        std::vector<ID3D11ShaderResourceView*> shader_resources;
        auto resources_itr = Resources.begin();
        auto resources_end = Resources.end();
        while(extractConsecutiveSlots(resources_itr, resources_end, slot, shader_resources)) {
            DC->VSSetShaderResources(slot, shader_resources.size(), shader_resources.data());
        }

        std::vector<ID3D11SamplerState*> samplers;
        auto samplers_itr = Samplers.begin();
        auto samplers_end = Samplers.end();
        while(extractConsecutiveSlots(samplers_itr, samplers_end, slot, samplers)) {
            DC->VSSetSamplers(slot, samplers.size(), samplers.data());
        }

        std::vector<ID3D11Buffer*> constants;
        auto constants_itr = Constants.begin();
        auto constants_end = Constants.end();
        while(extractConsecutiveSlots(constants_itr, constants_end, slot, constants)) {
            DC->VSSetConstantBuffers(slot, constants.size(), constants.data());
        }
    }

    bool isValid() const noexcept { return !!shader_; }
    ShaderType type() const noexcept override { return ShaderType::kVertexShader; }
private :
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
};

class PixelShader : public ShaderImpl
{
public :
    PixelShader(ID3D11Device* D3DDevice, const std::string& FilePath)
    {
        auto binary = loadBinary(FilePath);
        if(!binary.bytecode) return;

        D3DDevice->CreatePixelShader(binary.bytecode, binary.length, nullptr, &shader_);
        // エラーチェック してもしなくても処理が変わらないので、していない
    }

    void setShaderToContext(ID3D11DeviceContext* DC, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& Resources, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11SamplerState>>& Samplers, const std::map<UINT, Microsoft::WRL::ComPtr<ID3D11Buffer>>& Constants) override
    {
        DC->PSSetShader(shader_.Get(), nullptr, 0);
        
        UINT slot;

        std::vector<ID3D11ShaderResourceView*> shader_resources;
        auto resources_itr = Resources.begin();
        auto resources_end = Resources.end();
        while(extractConsecutiveSlots(resources_itr, resources_end, slot, shader_resources)) {
            DC->PSSetShaderResources(slot, shader_resources.size(), shader_resources.data());
        }

        std::vector<ID3D11SamplerState*> samplers;
        auto samplers_itr = Samplers.begin();
        auto samplers_end = Samplers.end();
        while(extractConsecutiveSlots(samplers_itr, samplers_end, slot, samplers)) {
            DC->PSSetSamplers(slot, samplers.size(), samplers.data());
        }

        std::vector<ID3D11Buffer*> constants;
        auto constants_itr = Constants.begin();
        auto constants_end = Constants.end();
        while(extractConsecutiveSlots(constants_itr, constants_end, slot, constants)) {
            DC->PSSetConstantBuffers(slot, constants.size(), constants.data());
        }
    }

    bool isValid() const noexcept { return !!shader_; }
    ShaderType type() const noexcept override { return ShaderType::kPixelShader; }
private :
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_;
};

} // unnamed namespace

//----------------------------------------------------------------------------- ShaderInfo
std::unique_ptr<ShaderImpl> VertexShaderInfo::createShaderImpl(ID3D11Device* D3DDevice) const
{
    return std::make_unique<VertexShader>(D3DDevice, shader_file_path_, input_description_);
}

std::unique_ptr<ShaderImpl> PixelShaderInfo::createShaderImpl(ID3D11Device* D3DDevice) const
{
    return std::make_unique<PixelShader>(D3DDevice, shader_file_path_);
}

//----------------------------------------------------------------------------- Shader
Shader::Shader(ID3D11Device* D3DDevice, const ShaderInfo& Info) 
{
    shader_impl_ = Info.createShaderImpl(D3DDevice);
}

void Shader::setShaderStageToContext(ID3D11DeviceContext* DC)
{
    assert(!!shader_impl_ && static_cast<bool>(shader_impl_->type()));
    shader_impl_->setShaderToContext(DC, resource_views_, samplers_, constants_);
}
// EOF
