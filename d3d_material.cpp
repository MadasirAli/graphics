#include "d3d_material.h"

#include "graphics_error.h"

using namespace base::graphics;

void d3d_material::bind(ID3D11DeviceContext* pContext, const d3d_sampler_col& samplers,
  const d3d_blend_col& blendStates) const
{
  D3D_CHECK_CALL(
    pContext->IASetInputLayout(_shader.get_input_layout())
  );

  D3D_CHECK_CALL(
    pContext->VSSetShader(_shader.get_vertex(), nullptr, 0)
  );
  D3D_CHECK_CALL(
    pContext->PSSetShader(_shader.get_pixel(), nullptr, 0)
  );

  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    const auto& textureBind = _vertexBindings.textures[i];
    const auto& cBufferBind = _vertexBindings.cBuffers[i];
    const auto& sBufferBind = _vertexBindings.sBuffers[i];
    const auto samplerBind = _vertexBindings.samplers[i];

    if (textureBind.first.first) {
      const uint32_t bindSlot = textureBind.second.second;
      ID3D11ShaderResourceView* pView = textureBind.second.first.get_shader_view();

      D3D_CHECK_CALL(
        pContext->VSSetShaderResources(bindSlot, 1u, &pView)
      );

      ID3D11SamplerState* pSampler = pSampler = samplers.get_sampler(
        samplerBind == sampler_mode::undef? textureBind.second.first.get_sampler() : samplerBind);
      D3D_CHECK_CALL(
        pContext->VSSetSamplers(bindSlot, 1, &pSampler)
      );
    }

    if (cBufferBind.first.first) {
      const uint32_t bindSlot = cBufferBind.second.second;
      ID3D11Buffer* pBuf = cBufferBind.second.first.get_ptr();

      D3D_CHECK_CALL(
        pContext->VSSetConstantBuffers(bindSlot, 1u, &pBuf)
      );
    }

    if (sBufferBind.first.first) {
      const uint32_t bindSlot = sBufferBind.second.second;
      ID3D11ShaderResourceView* pView = sBufferBind.second.first.get_shader_view();

      D3D_CHECK_CALL(
        pContext->VSSetShaderResources(bindSlot, 1u, &pView)
      );
    }
  }

  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    const auto& textureBind = _pixelBindsings.textures[i];
    const auto& cBufferBind = _pixelBindsings.cBuffers[i];
    const auto& sBufferBind = _pixelBindsings.sBuffers[i];
    const auto samplerBind = _pixelBindsings.samplers[i];

    if (textureBind.first.first) {
      const uint32_t bindSlot = textureBind.second.second;
      ID3D11ShaderResourceView* pView = textureBind.second.first.get_shader_view();

      D3D_CHECK_CALL(
        pContext->PSSetShaderResources(bindSlot, 1u, &pView)
      );


      ID3D11SamplerState* pSampler = pSampler = samplers.get_sampler(
        samplerBind == sampler_mode::undef ? textureBind.second.first.get_sampler() : samplerBind);
      D3D_CHECK_CALL(
        pContext->PSSetSamplers(bindSlot, 1, &pSampler)
      );
    }

    if (cBufferBind.first.first) {
      const uint32_t bindSlot = cBufferBind.second.second;
      ID3D11Buffer* pBuf = cBufferBind.second.first.get_ptr();

      D3D_CHECK_CALL(
        pContext->PSSetConstantBuffers(bindSlot, 1u, &pBuf)
      );
    }

    if (sBufferBind.first.first) {
      const uint32_t bindSlot = sBufferBind.second.second;
      ID3D11ShaderResourceView* pView = sBufferBind.second.first.get_shader_view();

      D3D_CHECK_CALL(
        pContext->PSSetShaderResources(bindSlot, 1u, &pView)
      );
    }
  }

  ID3D11BlendState* pBlend = blendStates.get_blend_state(_blendMode);
  D3D_CHECK_CALL(
    pContext->OMSetBlendState(pBlend, nullptr, D3D11_DEFAULT_SAMPLE_MASK)
  );
}

void d3d_material::set_texture(const std::string& name, const d3d_texture& texture, sampler_mode sampler)
{
  using namespace std;

  {
    const auto& reflect = _shader.get_vertex_reflection();

    for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
      if (strcmp(reflect.textures[i].first.data(), name.c_str()) == false) {
        _vertexBindings.textures[i] = make_pair(make_pair(true, reflect.textures[i].first), make_pair(texture, reflect.textures[i].second));
        _vertexBindings.samplers[i] = sampler;

        break;
      }
    }
  }
  {
    const auto& reflect = _shader.get_pixel_reflection();

    for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
      if (strcmp(reflect.textures[i].first.data(), name.c_str()) == false) {
        _pixelBindsings.textures[i] = make_pair(make_pair(true, reflect.textures[i].first), make_pair(texture, reflect.textures[i].second));
        _pixelBindsings.samplers[i] = sampler;

        break;
      }
    }
  }
}

void d3d_material::remove_texture(const std::string& name)
{
  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    if (strcmp(_vertexBindings.textures[i].first.second.data(), name.c_str()) == false) {
      _vertexBindings.textures[i].first.first = false;
      _vertexBindings.textures[i].second.first = d3d_texture{};
      _vertexBindings.samplers[i] = sampler_mode::undef;

      break;
    }
  }

  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    if (strcmp(_pixelBindsings.textures[i].first.second.data(), name.c_str()) == false) {
      _pixelBindsings.textures[i].first.first = false;
      _pixelBindsings.textures[i].second.first = d3d_texture{};
      _pixelBindsings.samplers[i] = sampler_mode::undef;
      
      break;
    }
  }
}

void d3d_material::set_cbuffer(const std::string& name, const d3d_buffer& buffer)
{
  using namespace std;

  {
    const auto& reflect = _shader.get_vertex_reflection();

    for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
      if (strcmp(reflect.cBuffers[i].first.data(), name.c_str()) == false) {
        _vertexBindings.cBuffers[i] = make_pair(make_pair(true, reflect.cBuffers[i].first), make_pair(buffer, reflect.cBuffers[i].second));
        break;
      }
    }
  }

  {
    const auto& reflect = _shader.get_pixel_reflection();

    for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
      if (strcmp(reflect.cBuffers[i].first.data(), name.c_str()) == false) {
        _pixelBindsings.cBuffers[i] = make_pair(make_pair(true, reflect.cBuffers[i].first), make_pair(buffer, reflect.cBuffers[i].second));
        break;
      }
    }
  }
}

void d3d_material::remove_cbuffer(const std::string& name)
{
  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    if (strcmp(_vertexBindings.cBuffers[i].first.second.data(), name.c_str()) == false) {
      _vertexBindings.cBuffers[i].first.first = false;
      _vertexBindings.cBuffers[i].second.first = d3d_buffer{};

      break;
    }
  }

  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    if (strcmp(_pixelBindsings.cBuffers[i].first.second.data(), name.c_str()) == false) {
      _pixelBindsings.cBuffers[i].first.first = false;
      _pixelBindsings.cBuffers[i].second.first = d3d_buffer{};

      break;
    }
  }
}

void d3d_material::set_sbuffer(const std::string& name, const d3d_buffer& buffer)
{
  using namespace std;

  {
    const auto& reflect = _shader.get_vertex_reflection();

    for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
      if (strcmp(reflect.sBuffers[i].first.data(), name.c_str()) == false) {
        _vertexBindings.sBuffers[i] = make_pair(make_pair(true, reflect.sBuffers[i].first), make_pair(buffer, reflect.sBuffers[i].second));
        break;
      }
    }
  }

  {
    const auto& reflect = _shader.get_pixel_reflection();

    for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
      if (strcmp(reflect.sBuffers[i].first.data(), name.c_str()) == false) {
        _pixelBindsings.sBuffers[i] = make_pair(make_pair(true, reflect.sBuffers[i].first), make_pair(buffer, reflect.sBuffers[i].second));
        break;
      }
    }
  }
}

void d3d_material::remove_sbuffer(const std::string& name)
{
  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    if (strcmp(_vertexBindings.sBuffers[i].first.second.data(), name.c_str()) == false) {
      _vertexBindings.sBuffers[i].first.first = false;
      _vertexBindings.sBuffers[i].second.first = d3d_buffer{};

      break;
    }
  }

  for (size_t i = 0; i < d3d_shader::reflection::max_enties; ++i) {
    if (strcmp(_pixelBindsings.sBuffers[i].first.second.data(), name.c_str()) == false) {
      _pixelBindsings.sBuffers[i].first.first = false;
      _pixelBindsings.sBuffers[i].second.first = d3d_buffer{};

      break;
    }
  }
}
