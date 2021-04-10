#pragma once

#include "level.hpp"
#include "render.hpp"
#include "shader.hpp"

class Sandbox : public Level
{
public :
    void initialize() override;
    void finalize() override;
    void update() override;

private :
    std::unique_ptr<RenderBatch> renderer_;
    GPUMesh mesh_;
    GPUMesh floor_mesh_;
};
// EOF
