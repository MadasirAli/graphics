#pragma once

#include "d3d_inc.h"

#include "sampler_mode.h"
#include "texture_type.h"
#include "access_mode.h"

namespace base {
  namespace graphics {
    class d3d_texture
    {
    public:
      d3d_texture() {};
      d3d_texture(uint32_t width, uint32_t height, const char* const* ppPixels,
        ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DXGI_FORMAT format,
        bool generateMips, access_mode access, texture_type type, uint32_t count, sampler_mode sampler);

      ID3D11Texture2D* get_ptr() const {
        return _pTexture.Get();
      }
      ID3D11ShaderResourceView* get_shader_view() const {
        return _pShaderView.Get();
      }
      ID3D11RenderTargetView* get_render_view() const {
        return _pRenderView.Get();
      }
      texture_type get_type() const {
        return _type;
      }
      DXGI_FORMAT get_format() const {
        return _format;
      }
      uint32_t get_width() const {
        return _width;
      }
      uint32_t get_height() const {
        return _height;
      }
      uint32_t get_count() const {
        return _count;
      }
      uint32_t get_pixel_size() const {
        return _pixelSize;
      }
      access_mode get_read_write() const {
        return _access;
      }
      sampler_mode get_sampler() const {
        return _sampler;
      }

    private:
      Microsoft::WRL::ComPtr<ID3D11Texture2D> _pTexture;
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pShaderView;
      Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRenderView;

      texture_type _type = texture_type::undef;
      DXGI_FORMAT _format = DXGI_FORMAT_UNKNOWN;
      uint32_t _width = 0;
      uint32_t _height = 0;
      uint32_t _count = 0;
      uint32_t _pixelSize = 0;
      access_mode _access = access_mode::undef;
      sampler_mode _sampler = sampler_mode::undef;
    public:
      static constexpr const size_t max_count = 32;
      static constexpr const size_t max_mips = 16;
    };
  }
}

