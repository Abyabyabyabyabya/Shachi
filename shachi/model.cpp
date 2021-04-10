#include "model.hpp"
#include <fstream>
#include "graphics.hpp"

namespace {
template <class DataType>
void createBuffer(ID3D11Device*, const std::vector<DataType>&, D3D11_BUFFER_DESC*, ID3D11Buffer**);
} // unnamed namespace


//----------------------------------------------------------------------------- GPUMesh
GPUMesh::GPUMesh(ID3D11Device* Device, const Mesh& Source, VertexType Type)
{
    assert(!!Device);

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;

	// create index-buffer
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	createBuffer(Device, Source.indices, &buffer_desc, &buffers_[kIndexBuffer]);

	// create vertex_buffer
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	switch (Type)
	{
	case VertexType::Position:
		createBuffer(Device, Source.positions, &buffer_desc, &buffers_[kVertexBufferPosition]);
		break;
	case VertexType::Position_UV:
		createBuffer(Device, Source.positions, &buffer_desc, &buffers_[kVertexBufferPosition]);
		createBuffer(Device, Source.uvs, &buffer_desc, &buffers_[kVertexBufferUV]);
		break;
	case VertexType::Position_Normal:
		createBuffer(Device, Source.positions, &buffer_desc, &buffers_[kVertexBufferPosition]);
		createBuffer(Device, Source.normals, &buffer_desc, &buffers_[kVertexBufferNormal]);
		break;
	case VertexType::Position_UV_Normal:
		createBuffer(Device, Source.positions, &buffer_desc, &buffers_[kVertexBufferPosition]);
		createBuffer(Device, Source.normals, &buffer_desc, &buffers_[kVertexBufferNormal]);
		createBuffer(Device, Source.uvs, &buffer_desc, &buffers_[kVertexBufferUV]);
		break;
	default:
		assert(false && "GPUMesh - Unknowm Vertex Type");
		break;
	}

	type_ = Type;
	num_indices_ = Source.indices.size();
}

//----------------------------------------------------------------------------- Function
void importMeshFromObjFile(const std::string& FilePath, Mesh* OutMesh, void* /*OutMaterial*/, bool IsRightHand)
{
	OutMesh->indices.clear();
	OutMesh->normals.clear();
	OutMesh->positions.clear();
	OutMesh->uvs.clear();

	std::fstream file{FilePath};
	assert(!!file);

	std::vector<MeshPosition> positions;
	std::vector<MeshUV>       uvs;
	std::vector<MeshNormal>   normals;

	std::string command;
	while(file >> command) {
		if(command == "v") {
			MeshPosition pos;
			file >> pos.x >> pos.y >> pos.z;
			if(IsRightHand) pos.z *= -1;
			positions.emplace_back(pos);
		}
		else if(command == "vt") {
			MeshUV uv;
			file >> uv.u >> uv.v;
			if(IsRightHand) uv.v = 1.0F - uv.v;
			uvs.emplace_back(uv);
		}
		else if(command == "vn") {
			MeshNormal norm;
			file >> norm.x >> norm.y >> norm.z;
			if(IsRightHand) norm.z *= -1.0F;
			normals.emplace_back(norm);
		}
		else if(command == "f") {
			const size_t kCurrVertsNum = OutMesh->indices.size();
			for(int i=0; i<3; ++i) {
				std::string data;
				file >> data;
				int pos, uv, norm;
				sscanf_s(data.c_str(), "%d/%d/%d", &pos, &uv, &norm);

				OutMesh->indices.emplace_back(kCurrVertsNum+2 - i); // 2-1-0 ‚Ì‡
				OutMesh->positions.emplace_back(positions[pos-1]);
				OutMesh->uvs.emplace_back(uvs[uv-1]);
				OutMesh->normals.emplace_back(normals[norm-1]);
			}
		}
	}
}
// EOF

namespace { template <class DataType> void createBuffer(ID3D11Device* Device, const std::vector<DataType>& Source, D3D11_BUFFER_DESC* Desc, ID3D11Buffer** OutBuffer)
{
	Desc->ByteWidth = sizeof(DataType) * Source.size();

	D3D11_SUBRESOURCE_DATA srd{};
	srd.pSysMem = Source.data();

	auto hr = Device->CreateBuffer(Desc, &srd, OutBuffer);
	assert(SUCCEEDED(hr));
}}
