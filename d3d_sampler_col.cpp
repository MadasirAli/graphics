#include "d3d_sampler_col.h"

#include "graphics_error.h"

using namespace base::graphics;

d3d_sampler_col::d3d_sampler_col(ID3D11Device* pDevice)
{
  D3D11_SAMPLER_DESC desc = {};
  desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

  desc.BorderColor[0] = 0;
  desc.BorderColor[1] = 0;
  desc.BorderColor[2] = 0;
  desc.BorderColor[3] = 0;

  desc.MaxLOD = D3D11_FLOAT32_MAX;

  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

  D3D_CALL(
    pDevice->CreateSamplerState(&desc, _pPointSampler.GetAddressOf())
  );

  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

  D3D_CALL(
    pDevice->CreateSamplerState(&desc, _pLinearSampler.GetAddressOf())
  );

  desc.Filter = D3D11_FILTER_ANISOTROPIC;
  desc.MaxAnisotropy = 16;

  D3D_CALL(
    pDevice->CreateSamplerState(&desc, _pAniosSampler.GetAddressOf())
  );
}

ID3D11SamplerState* d3d_sampler_col::get_sampler(sampler_mode mode) const
{
  assert(mode != sampler_mode::undef);
  
  if (mode == sampler_mode::linear) {
    return _pLinearSampler.Get();
  }
  else if (mode == sampler_mode::point) {
    return _pPointSampler.Get();
  }
  else if (mode == sampler_mode::aniostrophic) {
    return _pAniosSampler.Get();
  }
  else {
    assert(false);

    return nullptr;
  }
}
