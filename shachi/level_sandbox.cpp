#include "level_sandbox.hpp"
#include <cassert>
#include <DirectXMath.h>
#include "graphics.hpp"

namespace {

constexpr DirectX::XMFLOAT4 kColors[]
{
    { 0.8F, 0.8F, 0.8F, 1.0F },
    { 0.1F, 1.0F, 0.3F, 1.0F },
};

const VertexShaderInfo kVSInfo
{
    "Resource/simple_vs.cso",
    std::vector<D3D11_INPUT_ELEMENT_DESC> {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    }
};

const PixelShaderInfo kPSInfo
{
    "Resource/simple_ps.cso"
};

struct VertexConstantScene
{
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT4X4 view;
};
struct VertexConstantModel
{
    DirectX::XMFLOAT4X4 world;
};
struct PixelConstant
{
    DirectX::XMFLOAT4 color;
};

ConstantBuffer<PixelConstant> gPixelShaderConstant;

class SandboxRenderer : public RenderBatch
{
public :
    SandboxRenderer()
    {
        viewport_.TopLeftX = 0;
        viewport_.TopLeftY = 0;
        viewport_.Width = 1280;
        viewport_.Height = 720;
        viewport_.MinDepth = 0;
        viewport_.MaxDepth = 1;

        vertex_shader_ = Shader{Graphics::d3dDevice(), kVSInfo};
        if(!vertex_shader_) return;
        pixel_shader_ = Shader{Graphics::d3dDevice(), kPSInfo};
        if(!pixel_shader_) return;

        VertexConstantScene constant_scene;
        DirectX::XMStoreFloat4x4(&constant_scene.projection, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(70*DirectX::XM_PI/180.0F, 1280.0F/720.0F, 0.01F, 100.0F)) );
        DirectX::XMStoreFloat4x4(&constant_scene.view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH({ 0.0F, 1.0F, -3.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F})));
        vs_constant_scene_ = ConstantBuffer<VertexConstantScene>{Graphics::d3dDevice(), constant_scene};
        VertexConstantModel constant_model;
        DirectX::XMStoreFloat4x4(&constant_model.world, DirectX::XMMatrixIdentity());
        vs_constant_model_ = ConstantBuffer<VertexConstantModel>{Graphics::d3dDevice(), constant_model};
        gPixelShaderConstant = ConstantBuffer<PixelConstant>{Graphics::d3dDevice()};

        vertex_shader_.setConstantBuffer(vs_constant_scene_.buffer(), 0);
        vertex_shader_.setConstantBuffer(vs_constant_model_.buffer(), 1);
        pixel_shader_.setConstantBuffer(gPixelShaderConstant.buffer(), 0);
    }

    void begin(ID3D11DeviceContext* Context) override
    {
        assert(!!Context);

        context_ = Context;

        float color[4] = { 0.2F, 0.2F, 0.2F, 1.0F };
        context_->ClearRenderTargetView(Graphics::backbuffer(), color);

        context_->RSSetViewports(1, &viewport_);
        ID3D11RenderTargetView* rtv = Graphics::backbuffer();
        context_->OMSetRenderTargets(1, &rtv, nullptr);

        vertex_shader_.setShaderStageToContext(context_);
        pixel_shader_.setShaderStageToContext(context_);

        context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void renderMesh(const GPUMesh& Mesh) override
    {
        assert(!!context_);

        /*ここから作業ね。 ちなみにバッチ処理に変更してます。
        meshes_*/

        context_->IASetIndexBuffer(Mesh.indices(), DXGI_FORMAT_R32_UINT, 0);
        ID3D11Buffer* vertex_buffers = Mesh.positions();
        UINT strides = sizeof(MeshPosition);
        UINT offsets = 0;
        context_->IASetVertexBuffers(0, 1, &vertex_buffers, &strides, &offsets);

        context_->DrawIndexed(Mesh.numIndices(), 0, 0);
    }

    void end() override
    {

    }

private :
    size_t index_ {0};
    std::vector<GPUMesh> meshes_;
    D3D11_VIEWPORT viewport_;
    ID3D11DeviceContext* context_;
    ConstantBuffer<VertexConstantScene> vs_constant_scene_;
    ConstantBuffer<VertexConstantModel> vs_constant_model_;
    Shader vertex_shader_;
    Shader pixel_shader_;
};

} // unnamed namespace

void Sandbox::initialize()
{
    renderer_ = std::make_unique<SandboxRenderer>();

    Mesh mesh;
    importMeshFromObjFile("Resource/pukapuka_onomatope.obj", &mesh);
    mesh_ = GPUMesh{Graphics::d3dDevice(), mesh, VertexType::Position};
    importMeshFromObjFile("Resource/floor.obj", &mesh);
    floor_mesh_ = GPUMesh{Graphics::d3dDevice(), mesh, VertexType::Position};
}

void Sandbox::finalize()
{

}

void Sandbox::update()
{
    Graphics::beginRender();
// setup draw context
    auto context = Graphics::d3dImmediateContext();
    
    renderer_->begin(context);

    
    PixelConstant cb_ps;
    cb_ps.color = kColors[0];
    gPixelShaderConstant.update(context, cb_ps);
    renderer_->renderMesh(floor_mesh_);

    cb_ps.color = kColors[1];
    gPixelShaderConstant.update(context, cb_ps);
    renderer_->renderMesh(mesh_);

    renderer_->end();

//!setup draw context
    Graphics::endRender();
}
// EOF
