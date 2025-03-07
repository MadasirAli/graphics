#pragma once

#include "d3d_inc.h"

#include "sampler_mode.h"

namespace base {
  namespace graphics {
    class d3d_sampler_col
    {
    public:
      d3d_sampler_col() {};
      d3d_sampler_col(ID3D11Device* pDevice);

      ID3D11SamplerState* get_sampler(sampler_mode mode) const;

    private:
      Microsoft::WRL::ComPtr<ID3D11SamplerState> _pPointSampler;
      Microsoft::WRL::ComPtr<ID3D11SamplerState> _pLinearSampler;
      Microsoft::WRL::ComPtr<ID3D11SamplerState> _pAniosSampler;
    };
  }
}

