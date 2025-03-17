#include "d3d_mesh.h"

#include "graphics_error.h"

using namespace base::graphics;

d3d_mesh::d3d_mesh(const index* indices, uint32_t indexCount, const vertex* vertices, uint32_t vertexCount, ID3D11Device* pDevice)
{
  D3D11_BUFFER_DESC desc = { 0 };
  D3D11_SUBRESOURCE_DATA initData = { 0 };

  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  desc.StructureByteStride = sizeof(vertex);
  desc.ByteWidth = desc.StructureByteStride * vertexCount;

  initData.pSysMem = vertices;

  D3D_CALL(
    pDevice->CreateBuffer(&desc, &initData, _pVertexBuffer.GetAddressOf())
    );

  desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  desc.StructureByteStride = sizeof(index);
  desc.ByteWidth = desc.StructureByteStride * indexCount;

  initData.pSysMem = indices;

  D3D_CALL(
    pDevice->CreateBuffer(&desc, &initData, _pIndexBuffer.GetAddressOf())
  );

  _indexCount = (uint32_t)indexCount;
  _vertexCount = (uint32_t)vertexCount;
}

void d3d_mesh::bind(ID3D11DeviceContext* pContext) const
{
  D3D_CHECK_CALL(
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
  );
  const UINT pStride = sizeof(vertex);
  const UINT pOffset = 0u;

  D3D_CHECK_CALL(
    pContext->IASetVertexBuffers(0, 1, _pVertexBuffer.GetAddressOf(), &pStride, &pOffset)
  );
  D3D_CHECK_CALL(
    pContext->IASetIndexBuffer(_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u)
  );
}

const d3d_mesh d3d_mesh::quad(ID3D11Device* pDevice)
{
  vertex quadVertices[4] = {
    // Position, UV
    { { -1.0f, -1.0f }, { 0.0f, 1.0f } }, // Bottom-left (UV Y inverted)
    { {  1.0f, -1.0f }, { 1.0f, 1.0f } }, // Bottom-right (UV Y inverted)
    { { -1.0f,  1.0f }, { 0.0f, 0.0f } }, // Top-left (UV Y inverted)
    { {  1.0f,  1.0f }, { 1.0f, 0.0f } }  // Top-right (UV Y inverted)
  };

  index quadIndices[6] = {
      0, 2, 1, // First triangle (bottom-left, top-left, bottom-right)
      1, 2, 3  // Second triangle (bottom-right, top-left, top-right)
  };

  static d3d_mesh quad(quadIndices, 6, quadVertices, 4, pDevice);

  return quad;
}
