#pragma once

#include <vector>
#include <d3d11.h>
#include "model.hpp"

class Renderer
{
public :
    virtual ~Renderer() = default;
    virtual void renderMesh(const GPUMesh& Mesh) = 0;
};

class RenderBatch : public Renderer
{
public :
    virtual void begin(ID3D11DeviceContext* D3DContext) = 0;
    virtual void end() = 0;
};
//
//class BatchRender final : public Renderer
//{
//public :
//    void renderMesh(const GPUMesh& Mesh) override;
//    void batch(ID3D11DeviceContext* Context, RenderPass* Pass);
//private :
//    size_t index_ {0U};
//    std::vector<GPUMesh> meshes_;
//};
// EOF
