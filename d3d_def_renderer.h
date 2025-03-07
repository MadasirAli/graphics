#pragma once

#include "d3d_inc.h"

#include "d3d_mesh.h"
#include "d3d_material.h"

namespace base {
  namespace graphics {
    class d3d_def_renderer
    {
      friend class d3d_renderer;

    public:
      d3d_def_renderer(ID3D11Device* pDevice, const d3d_sampler_col& samplers, const d3d_blend_col& blends,
        const d3d_mesh& mesh, const d3d_texture& renderTexture, const D3D11_VIEWPORT& viewport);

      void draw_quad(const d3d_material& material) const;
      void draw_quad_instanced(const d3d_material& material, uint32_t count) const;

      void draw(const d3d_material& material, const d3d_mesh& mesh) const;
      void draw_instanced(const d3d_material& material, const d3d_mesh& quad, uint32_t count) const;

    private:
      void clear_state() const;

    private:
      Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pContext;

      d3d_sampler_col _samplers;
      d3d_blend_col _blends;
      d3d_mesh _quad;

      d3d_texture _renderTexture;
      D3D11_VIEWPORT _viewport;
    };
  }
}