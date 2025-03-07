#pragma once
#include <cstdint>
#include <array>
#include <utility>

#include "d3d_inc.h"

namespace base {
  namespace graphics {
    class d3d_shader
    {
    public:
      struct reflection {
        constexpr static size_t name_len = 32;
        constexpr static size_t max_enties = 32;

        std::array<std::pair<std::array<char, name_len>, uint16_t>, max_enties> sBuffers;
        std::array<std::pair<std::array<char, name_len>, uint16_t>, max_enties> cBuffers;
        std::array<std::pair<std::array<char, name_len>, uint16_t>, max_enties> textures;
        std::array<std::pair<std::array<char, name_len>, uint16_t>, max_enties> samplers;
      };
      enum class flags {
        undef = 0,
        vertex = 1 << 0,
        pixel = 1 << 1,
      };
    public:
      d3d_shader(const char* bytes, size_t length, ID3D11Device* pDevice);

      ID3D11VertexShader* get_vertex() const {
        return _pVertexShader.Get();
      }
      ID3D11PixelShader* get_pixel() const {
        return _pPixelShader.Get();
      }
      const reflection& get_vertex_reflection() const {
        return _vertexReflect;
      }
      const reflection& get_pixel_reflection() const {
        return _pixelReflect;
      }
      ID3D11InputLayout* get_input_layout() const {
        return _pInputLayout.Get();
      }

    private:
      void reflect(ID3D11ShaderReflection* pReflector, ID3D11Device* pDevice, ID3DBlob* pVertex,
        reflection& reflect_out);

    private:
      uint32_t _flags = 0;
      Microsoft::WRL::ComPtr<ID3D11VertexShader> _pVertexShader;
      Microsoft::WRL::ComPtr<ID3D11PixelShader> _pPixelShader;
      Microsoft::WRL::ComPtr<ID3D11InputLayout> _pInputLayout;

      reflection _vertexReflect = {};
      reflection _pixelReflect = {};
    };
  }
}