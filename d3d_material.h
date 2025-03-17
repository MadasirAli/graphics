#pragma once

#include "d3d_inc.h"
#include <string>

#include "d3d_shader.h"
#include "d3d_texture.h"
#include "d3d_buffer.h"
#include "d3d_sampler_col.h"
#include "d3d_blend_col.h"

namespace base {
  namespace graphics {
    class d3d_material
    {
    private:
      struct bindings {
        std::array <
          std::pair <
            std::pair<bool, std::array<char, d3d_shader::reflection::name_len>>,
            std::pair<d3d_texture, uint16_t>
          >,
        d3d_shader::reflection::max_enties> textures;
        std::array<sampler_mode, d3d_shader::reflection::max_enties>  samplers;

        std::array <
          std::pair <
            std::pair<bool, std::array<char, d3d_shader::reflection::name_len>>,
            std::pair<d3d_buffer, uint16_t>
          >,
        d3d_shader::reflection::max_enties> cBuffers;;

        std::array <
          std::pair <
            std::pair<bool, std::array<char, d3d_shader::reflection::name_len>>,
            std::pair<d3d_buffer, uint16_t>
          >,
        d3d_shader::reflection::max_enties> sBuffers;
      };
    public:
      d3d_material(const d3d_shader& shader) :
        _shader(shader)
      {}
      d3d_material() {}

      void bind(ID3D11DeviceContext* pContext, const d3d_sampler_col& samplers,
        const d3d_blend_col& blendStates) const;

      void set_texture(const std::string& name, const d3d_texture& texture, sampler_mode sampler);
      void remove_texture(const std::string& name);
      void set_cbuffer(const std::string& name, const d3d_buffer& buffer);
      void remove_cbuffer(const std::string& name);
      void set_sbuffer(const std::string& name, const d3d_buffer& buffer);
      void remove_sbuffer(const std::string& name);

      void set_blend(blend_mode mode) {
        assert(mode != blend_mode::undef);
        _blendMode = mode;
      }

    private:
      d3d_shader _shader;
      bindings _vertexBindings = {};
      bindings _pixelBindsings = {};
      blend_mode _blendMode = blend_mode::off;
    };
  }
}

