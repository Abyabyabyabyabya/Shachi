#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <d3d11.h>
#include <wrl.h>

enum class VertexType
{
    Position,
    Position_UV,
    Position_Normal,
    Position_UV_Normal,
};

struct MeshPosition
{
    float x;
    float y;
    float z;
};

struct MeshNormal
{
    float x;
    float y;
    float z;
};

struct MeshUV
{
    float u;
    float v;
};

struct Mesh
{
    std::vector<uint32_t>     indices;
    std::vector<MeshPosition> positions;
    std::vector<MeshUV>       uvs;
    std::vector<MeshNormal>   normals;
};

class GPUMesh
{
private :
    enum Index {
        kIndexBuffer,
        kVertexBufferPosition,
        kVertexBufferUV,
        kVertexBufferNormal,
    };
public :
    GPUMesh() = default;
    GPUMesh(ID3D11Device* D3DDevice, const Mesh& Source, VertexType);

    VertexType vertexType() const noexcept { return type_; }
    size_t numIndices() const noexcept { return num_indices_; }
    ID3D11Buffer* indices() const noexcept { return buffers_[kIndexBuffer].Get(); }
    ID3D11Buffer* positions() const noexcept { return buffers_[kVertexBufferPosition].Get(); }
    ID3D11Buffer* uvs() const noexcept { return buffers_[kVertexBufferUV].Get(); }
    ID3D11Buffer* normals() const noexcept { return buffers_[kVertexBufferNormal].Get(); }
private :
    VertexType type_;
    size_t num_indices_;
    std::array<Microsoft::WRL::ComPtr<ID3D11Buffer>, 4U> buffers_;
};

void importMeshFromObjFile(const std::string& Path, Mesh*, void* /*Material*/=nullptr, bool IsRightHand=true);
// EOF
