#pragma once

#include "d3d_inc.h"

#include "blend_mode.h"

namespace base {
  namespace graphics {
    class d3d_blend_col
    {
    public:
      d3d_blend_col() {};
      d3d_blend_col(ID3D11Device* pDevice);

      ID3D11BlendState* get_blend_state(blend_mode mode) const;

    private:
      Microsoft::WRL::ComPtr<ID3D11BlendState> _pOff;
      Microsoft::WRL::ComPtr<ID3D11BlendState> _pOn;
    };
  }
}

