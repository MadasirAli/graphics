#include "d3d_buffer.h"

#include "graphics_error.h"

using namespace base::graphics;

d3d_buffer::d3d_buffer(const char* pData, uint32_t length, buffer_type type, 
  ID3D11Device* pDevice, uint32_t count, access_mode access)
{
  D3D11_BIND_FLAG bindFlags = (D3D11_BIND_FLAG)0;
  UINT cpuFlags = 0;
  UINT miscFlags = 0;
  D3D11_USAGE usage = (D3D11_USAGE)0;

  if (type == buffer_type::constant) {
    bindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
  }
  else if (type == buffer_type::structured) {
    bindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
    miscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
  }
  else if (type == buffer_type::staging) {
    bindFlags = (D3D11_BIND_FLAG)0;
    usage = D3D11_USAGE::D3D11_USAGE_STAGING;
  }
  else {
    assert(false);
  }


  if (access == access_mode::immutable) {
    usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
    cpuFlags = 0;
  }
  else if (access == access_mode::none) {
    usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
  }
  else if (access == access_mode::write_discard) {
    usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    cpuFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else if (access == access_mode::read) {
    assert(type == buffer_type::staging);
    cpuFlags = D3D11_CPU_ACCESS_READ;
  }
  else if (access == access_mode::write) {
    assert(type == buffer_type::staging || type == buffer_type::structured);
    cpuFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else if (access == access_mode::read_write) {
    //assert(type == buffer_type::staging);
    cpuFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
  }
  else {
    assert(false);
  }

  D3D11_BUFFER_DESC desc = { 0 };
  D3D11_SUBRESOURCE_DATA initData = { 0 };

  assert(count > 0);

  desc.ByteWidth = length * count;
  desc.BindFlags = bindFlags;
  desc.Usage = usage;
  desc.CPUAccessFlags = cpuFlags;
  desc.MiscFlags = miscFlags;
  desc.StructureByteStride = length;
  
  initData.pSysMem = pData;

  D3D_CALL(
    pDevice->CreateBuffer(&desc, pData? &initData : nullptr, _pBuffer.GetAddressOf())
  );

  if (type == buffer_type::structured) {
    assert(count > 0);

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    viewDesc.Format = DXGI_FORMAT_UNKNOWN;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

    viewDesc.Buffer.ElementOffset = 0;
    viewDesc.Buffer.NumElements = count;

    D3D_CALL(
      pDevice->CreateShaderResourceView(_pBuffer.Get(), &viewDesc, _pShaderView.GetAddressOf())
    );
  }

  _type = type;
  _length = length;
  _count = count;
  _access = access;
}