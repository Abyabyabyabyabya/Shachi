#include "render.hpp"
//
//void BatchRender::renderMesh(const GPUMesh& Mesh)
//{
//    if(index_ >= meshes_.size())
//        meshes_.emplace_back(Mesh);
//    else
//        meshes_[index_++] = Mesh;
//}
//
//void BatchRender::batch(ID3D11DeviceContext* Context, RenderPass* Pass)
//{
//    Pass->prepareForRenderMesh(Context);
//    for(size_t i=0U; i<index_; ++i)
//        Pass->renderMesh(meshes_[i]);
//    
//    index_ = 0U;
//}
// EOF
