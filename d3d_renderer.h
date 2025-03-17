#pragma once

#include "d3d_inc.h"
#include <array>

#include "d3d_buffer.h"
#include "d3d_texture.h"
#include "d3d_mesh.h"
#include "d3d_material.h"
#include "d3d_def_renderer.h"

namespace base {
  namespace graphics {

    class d3d_renderer
    {
    public:
      d3d_renderer(HWND hwnd);
      d3d_renderer(const d3d_renderer&) = delete;
      d3d_renderer& operator=(const d3d_renderer&) = delete;

      void draw_quad(const d3d_material& material) const;
      void draw_quad_instanced(const d3d_material& material, uint32_t count) const;

      void draw(const d3d_material& material, const d3d_mesh& mesh) const;
      void draw_instanced(const d3d_material& material, const d3d_mesh& mesh, uint32_t count) const;

      /* Currently not in use */
      //const d3d_texture& get_render_target() const {
      //  return _renderTexture;
      //}
      //void set_render_target(const d3d_texture& texture) {
      //  _renderTexture = texture;
      //}

      void resize(uint32_t width, uint32_t height);

      void play(const d3d_def_renderer& renderer) const;

      void present(bool vSync = false) const;
      void clear_render_target(const std::array<float, 4>& color);

      void map_buffer(const d3d_buffer& buffer, D3D11_MAPPED_SUBRESOURCE& map_out) const;
      /* Currently not in use */
      //void map_texture(const d3d_texture& texture, D3D11_MAPPED_SUBRESOURCE& map_out) const;

      void unmap_buffer(const d3d_buffer& buffer) const;
      /* Currently not in use */
      //void unmap_texture(const d3d_texture& texture) const;

      /* Currently not in use */
      //void update_texture(const d3d_texture& texture, const char* pData, uint32_t subIndex) const;
      void update_buffer(const d3d_buffer& buffer, const char* pData) const;

      void copy_buffer_region(const d3d_buffer& source, const d3d_buffer destination,
        uint32_t srcX, uint32_t srcY, uint32_t dstX, uint32_t dstY, uint32_t width, uint32_t height) const;

      void set_render_texture(const d3d_texture& renderTexture);
      const d3d_texture& get_render_texture() const;

      d3d_texture create_texture(uint32_t width, uint32_t height, const char* const* ppPixels,
        DXGI_FORMAT format, bool generateMips, access_mode access, texture_type type,
        uint32_t count, sampler_mode sampler) const;

      d3d_buffer create_buffer(
        const char* pData, uint32_t length, buffer_type type,uint32_t count, access_mode access) const;
      d3d_shader create_shader(const char* pBytes, size_t length) const;

      d3d_def_renderer create_def_renderer() const;

      void imgui_new_frame() const;
      void imgui_draw() const;
      bool imgui_forward_msg(MSG& msg);

    private:
      void clear_state() const;

    private:
      Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapchain;
      Microsoft::WRL::ComPtr<ID3D11Device> _pDevice;
      Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pContext;
      Microsoft::WRL::ComPtr<ID3D11Texture2D> _pBackBuffer;
      D3D11_VIEWPORT _viewport = { 0 };

      d3d_texture _renderTexture;

      d3d_sampler_col _samplers;
      d3d_blend_col _blends;

      d3d_mesh _quad;

      static constexpr const uint32_t _bufferCount = 2;
      static constexpr const uint32_t _swapchainFlags = 0;
    public:
      static constexpr const DXGI_FORMAT outputFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    };
  }
}

