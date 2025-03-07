#include "d3d_texture.h"
#include <array>
#include <cmath>

#ifdef max
#undef max
#endif

#include "graphics_error.h"

using namespace base::graphics;

d3d_texture::d3d_texture(uint32_t width, uint32_t height, const char* const* ppPixels, 
  ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DXGI_FORMAT format, 
  bool generateMips, access_mode access, texture_type type, uint32_t count, sampler_mode sampler)
{
  assert((count > 0 && count <= max_count));
  if (generateMips) {
    assert(ppPixels != nullptr);
    assert(access != access_mode::immutable);
  }

  D3D11_BIND_FLAG bindFlags = (D3D11_BIND_FLAG)0;
  UINT cpuFlags = 0;
  D3D11_USAGE usage = (D3D11_USAGE)0;

  if (type == texture_type::normal) {
    bindFlags = (D3D11_BIND_FLAG)(bindFlags | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE);
  }
  else if (type == texture_type::render) {
    bindFlags = (D3D11_BIND_FLAG)(bindFlags | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET);
  }
  else if (type == texture_type::staging) {
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
    assert(type == texture_type::staging);
    cpuFlags = D3D11_CPU_ACCESS_READ;
  }
  else if (access == access_mode::write) {
    assert(type == texture_type::staging);
    cpuFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else if (access == access_mode::read_write) {
    assert(type == texture_type::staging);
    cpuFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
  }
  else {
    assert(false);
  }

  D3D11_TEXTURE2D_DESC desc = { 0 };
  desc.Format = format;
  desc.BindFlags = bindFlags;
  desc.Usage = usage;
  desc.CPUAccessFlags = cpuFlags;
  desc.ArraySize = count;
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = generateMips ? 0 : 1;
  desc.SampleDesc.Count = 1;

  if (generateMips) {
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
  }

  _pixelSize = ((uint32_t)(DirectXUtil::BitsPerPixel(format)) / 8);
  uint32_t mipLevels = (uint32_t)(std::floor(std::log2(std::max(width, height)))) + 1;
  assert(mipLevels <= max_mips);

  std::array<D3D11_SUBRESOURCE_DATA, max_count> initData = { 0 };
  if (ppPixels != nullptr)
  {
    for (size_t i = 0; i < count; ++i)
    {
      initData[i].pSysMem = ppPixels[i];
      initData[i].SysMemPitch = _pixelSize * width;
      initData[i].SysMemSlicePitch = initData[i].SysMemPitch * height;
    }
  }

  D3D_CALL(
    pDevice->CreateTexture2D(&desc,
      (ppPixels && !generateMips)? initData.data() : nullptr, _pTexture.GetAddressOf())
  );

  switch (type) {
  case texture_type::render:
  {
    D3D11_RENDER_TARGET_VIEW_DESC viewDesc = {};
    viewDesc.Format = format;

    if (count > 1)
    {
      viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
      viewDesc.Texture2DArray.ArraySize = count;
      viewDesc.Texture2DArray.FirstArraySlice = 0;
      viewDesc.Texture2DArray.MipSlice = 0;

    }
    else {
      viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      viewDesc.Texture2D.MipSlice = 0;
    }


    D3D_CALL(
      pDevice->CreateRenderTargetView(_pTexture.Get(), &viewDesc, _pRenderView.GetAddressOf())
    );
  }
  break;

  case texture_type::normal:
  {
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    viewDesc.Format = format;
    if (count > 1)
    {
      viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
      viewDesc.Texture2DArray.ArraySize = count;
      viewDesc.Texture2DArray.MipLevels = -1;
      viewDesc.Texture2DArray.MostDetailedMip = 0;
      viewDesc.Texture2DArray.FirstArraySlice = 0;

    }
    else {
      viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      viewDesc.Texture2D.MipLevels = -1;  // view on all mip maps
      viewDesc.Texture2D.MostDetailedMip = 0;
    }

    D3D_CALL(
      pDevice->CreateShaderResourceView(_pTexture.Get(), &viewDesc, _pShaderView.GetAddressOf())
    );

    if (generateMips)
    {
      for (size_t i = 0; i < count; ++i)
      {
        D3D_CHECK_CALL(
          pContext->UpdateSubresource(_pTexture.Get(), i * mipLevels, nullptr,
            initData[i].pSysMem, initData[i].SysMemPitch, initData[i].SysMemSlicePitch)
        );
      }

      D3D_CHECK_CALL(
        pContext->GenerateMips(_pShaderView.Get())
      );
    }
  }
    break;
  case texture_type::staging:
    break;
  default:
    assert(false);
    break;
  }

  _type = type;
  _format = format;
  _width = width;
  _height = height;
  _count = count;
  _access = access;
  _sampler = sampler;
}