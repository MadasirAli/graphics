#include "d3d_def_renderer.h"

#include "graphics_error.h"

using namespace base::graphics;

void d3d_def_renderer::draw(const d3d_material& material, const d3d_mesh& mesh) const
{
  clear_state();

  mesh.bind(_pContext.Get());
  material.bind(_pContext.Get(), _samplers, _blends);

  D3D_CHECK_CALL(
    _pContext->DrawIndexed(mesh.get_indices_count(), 0, 0)
  );
}

void d3d_def_renderer::draw_instanced(const d3d_material& material, const d3d_mesh& mesh, uint32_t count) const
{
  clear_state();

  mesh.bind(_pContext.Get());
  material.bind(_pContext.Get(), _samplers, _blends);

  D3D_CHECK_CALL(
    _pContext->DrawIndexedInstanced(mesh.get_indices_count(), count, 0, 0, 0)
  );
}

d3d_def_renderer::d3d_def_renderer(ID3D11Device* pDevice, const d3d_sampler_col& samplers, const d3d_blend_col& blends,
  const d3d_mesh& quad, const d3d_texture& renderTexture, const D3D11_VIEWPORT& viewport) :
  _samplers(samplers),
  _blends(blends),
  _quad(quad),
  _renderTexture(renderTexture),
  _viewport(viewport)
{
  D3D_CALL(
    pDevice->CreateDeferredContext(0u, _pContext.GetAddressOf())
  );
}

void d3d_def_renderer::clear_state() const
{
  D3D_CHECK_CALL(
    _pContext->ClearState()
  );

  ID3D11RenderTargetView* pView = _renderTexture.get_render_view();
  D3D_CHECK_CALL(
    _pContext->OMSetRenderTargets(1, &pView, nullptr)
  );

  D3D_CHECK_CALL(
    _pContext->RSSetViewports(1, &_viewport);
  );
}

void d3d_def_renderer::draw_quad(const d3d_material& material) const
{
  this->draw(material, _quad);
}

void d3d_def_renderer::draw_quad_instanced(const d3d_material& material, uint32_t count) const
{
  this->draw_instanced(material, _quad, count);
}

