#pragma once

#include "d3d_inc.h"

#include "buffer_type.h"
#include "access_mode.h"

namespace base {
  namespace graphics {
    class d3d_buffer
    {
    public:
      d3d_buffer() {}
      d3d_buffer(const char* pData, uint32_t length, buffer_type type,
        ID3D11Device* pDevice, uint32_t count, access_mode access);

      ID3D11Buffer* get_ptr() const {
        return _pBuffer.Get();
      }
      ID3D11ShaderResourceView* get_shader_view() const {
        assert(_type == buffer_type::structured);
        return _pShaderView.Get();
      }
      buffer_type get_type() const {
        return _type;
      }
      access_mode get_read_write() const {
        return _access;
      }
      uint32_t get_length() const {
        return _length;
      }
      uint32_t get_count() const {
        return _count;
      }

    private:
      Microsoft::WRL::ComPtr<ID3D11Buffer> _pBuffer;
      Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pShaderView;

      buffer_type _type = buffer_type::undef;
      uint32_t _length = 0;
      uint32_t _count = 0;
      access_mode _access = access_mode::undef;
    };
  }
}

