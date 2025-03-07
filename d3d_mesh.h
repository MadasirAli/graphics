#pragma once
#include "d3d_inc.h"

#include <cstdint>
#include <array>

namespace base {
  namespace graphics {
    class d3d_mesh
    {
    public:
      struct vertex {
        float position[2] = { 0 };
        float uv[2] = { 0 };
      };
      struct index {
        uint16_t value = 0;
      };
    public:
      d3d_mesh() {}
      d3d_mesh(const index* indices, uint32_t indexCount, const vertex* vertices, uint32_t vertexCount, ID3D11Device* pDevice);
      void bind(ID3D11DeviceContext* pContext) const;

      const d3d_mesh static quad(ID3D11Device* pDevice);

      ID3D11Buffer* get_vertex_buf() const {
        return _pVertexBuffer.Get();
      }
      ID3D11Buffer* get_index_buf() const {
        return _pIndexBuffer.Get();
      }
      uint32_t get_indices_count() const {
        return _indexCount;
      }
      uint32_t get_vertices_count() const {
        return _vertexCount;
      }

    private:
      Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBuffer;
      Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBuffer;

      uint32_t _indexCount = 0;
      uint32_t _vertexCount = 0;
    };
  }
}