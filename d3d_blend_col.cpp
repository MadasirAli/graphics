#include "d3d_blend_col.h"

#include "graphics_error.h"

using namespace base::graphics;

d3d_blend_col::d3d_blend_col(ID3D11Device* pDevice)
{
  D3D11_BLEND_DESC desc = {0};
  desc.AlphaToCoverageEnable = false;

  desc.RenderTarget[0].BlendEnable = FALSE;
  desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  D3D_CALL(
    pDevice->CreateBlendState(&desc, _pOff.GetAddressOf())
  );

  desc.RenderTarget[0].BlendEnable = TRUE;  // Enable blending

  D3D_CALL(
    pDevice->CreateBlendState(&desc, _pOn.GetAddressOf())
  );
}

ID3D11BlendState* d3d_blend_col::get_blend_state(blend_mode mode) const
{
  assert(mode != blend_mode::undef);

  if (mode == blend_mode::off) {
    return _pOff.Get();
  }
  else if (mode == blend_mode::on) {
    return _pOn.Get();
  }
  else {
    assert(false);
  }

  return nullptr;
}
