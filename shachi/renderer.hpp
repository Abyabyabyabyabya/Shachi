#ifndef INCLUDED_SHACHI_RENDERER_HPP_
#define INCLUDED_SHACHI_RENDERER_HPP_

#include <d3d11.h>
#include <wrl.h>

///
/// @brief  �����_��
///         �`�惌�C���[
/// 
class Renderer
{
public :
    Renderer(ID3D11DeviceContext* DeviceContext);

private :
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context_;
};

#endif // !INCLUDED_SHACHI_RENDERER_HPP_
// EOF
