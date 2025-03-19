#include "d3d_renderer.h"

#include "graphics_error.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

using namespace base::graphics;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void d3d_renderer::draw(const d3d_material& material, const d3d_mesh& mesh) const
{
  clear_state();

  mesh.bind(_pContext.Get());
  material.bind(_pContext.Get(), _samplers, _blends);

  D3D_CHECK_CALL(
    _pContext->DrawIndexed(mesh.get_indices_count(), 0, 0)
  );
}

void d3d_renderer::draw_instanced(const d3d_material& material, const d3d_mesh& mesh, uint32_t count) const
{
  clear_state();

  mesh.bind(_pContext.Get());
  material.bind(_pContext.Get(), _samplers, _blends);

  D3D_CHECK_CALL(
    _pContext->DrawIndexedInstanced(mesh.get_indices_count(), count, 0, 0, 0)
  );
}

void d3d_renderer::play(const d3d_def_renderer& renderer) const
{
  Microsoft::WRL::ComPtr<ID3D11CommandList> pList{};
  D3D_CALL(
    renderer._pContext->FinishCommandList(false, pList.GetAddressOf())
  );
  D3D_CHECK_CALL(
    _pContext->ExecuteCommandList(pList.Get(), false)
  );
}

void d3d_renderer::clear_render_target(const std::array<float, 4>& color)
{
  ID3D11RenderTargetView* pView = _renderTexture.get_render_view();

  D3D_CHECK_CALL(
    _pContext->ClearRenderTargetView(pView, color.data())
  );
}

void d3d_renderer::present(bool vSync) const
{
  D3D_CHECK_CALL(
    _pContext->CopyResource(_pBackBuffer.Get(), _renderTexture.get_ptr())
  );

  D3D_CHECK_CALL(
    _pSwapchain->Present((UINT)vSync, 0)
  );
}

void d3d_renderer::resize(uint32_t width, uint32_t height) {
  _pBackBuffer.Reset();

  D3D_CALL(
    _pSwapchain->ResizeBuffers(_bufferCount, width, height, outputFormat, _swapchainFlags);
  );
  D3D_CALL(
    _pSwapchain->GetBuffer(0, IID_PPV_ARGS(_pBackBuffer.GetAddressOf()))
  );

  _renderTexture = create_texture(width, height, nullptr,
    outputFormat, false, access_mode::none, texture_type::render, 1, sampler_mode::point);

  _viewport.Width = (float)width;
  _viewport.Height = (float)height;
}

void d3d_renderer::set_render_texture(const d3d_texture& renderTexture) {
  _renderTexture = renderTexture;

  _viewport.Width = (float)renderTexture.get_width();
  _viewport.Height = (float)renderTexture.get_height();
}
const d3d_texture& d3d_renderer::get_render_texture() const {
  return _renderTexture;
}

void d3d_renderer::map_buffer(const d3d_buffer& buffer, D3D11_MAPPED_SUBRESOURCE& map_out) const
{
  assert(buffer.get_type() != buffer_type::undef);

  D3D11_MAP map = (D3D11_MAP)0;
  if (buffer.get_read_write() == access_mode::write_discard) {
    map = D3D11_MAP_WRITE_DISCARD;
  }
  else if (buffer.get_read_write() == access_mode::read) {
    map = D3D11_MAP_READ;
  }
  else if (buffer.get_read_write() == access_mode::write) {
    map = D3D11_MAP_WRITE;
  }
  else if (buffer.get_read_write() == access_mode::read_write) {
    map = (D3D11_MAP)(D3D11_MAP_WRITE | D3D11_MAP_READ);
  }
  else {
    assert(false);
  }

  map_out.pData = nullptr;
  D3D_CALL(
    _pContext->Map(buffer.get_ptr(), 0, map, 0, &map_out)
  );

  assert(map_out.pData != nullptr);
  if (map_out.pData == nullptr) {
    THROW_GERROR("Failed to map buffer.");
  }
}

/* Currently not in use */
//void d3d_renderer::map_texture(const d3d_texture& texture, D3D11_MAPPED_SUBRESOURCE& map_out) const
//{
//  D3D11_MAP map = (D3D11_MAP)0;
//  if (texture.get_read_write() == access_mode::write_discard) {
//    map = D3D11_MAP_WRITE_DISCARD;
//  }
//  else if (texture.get_read_write() == access_mode::read) {
//    map = D3D11_MAP_READ;
//  }
//  else if (texture.get_read_write() == access_mode::write) {
//    map = D3D11_MAP_WRITE;
//  }
//  else if (texture.get_read_write() == access_mode::read_write) {
//    map = (D3D11_MAP)(D3D11_MAP_WRITE | D3D11_MAP_READ);
//  }
//  else {
//    assert(false);
//  }
//
//  D3D_CALL(
//    _pContext->Map(texture.get_ptr(), 0, map, 0, &map_out)
//  );
//
//  assert(map_out.pData != nullptr);
//}

void d3d_renderer::unmap_buffer(const d3d_buffer& buffer) const
{
  _pContext->Unmap(buffer.get_ptr(), 0);
}
/* Currently not in use */
//void d3d_renderer::unmap_texture(const d3d_texture& texture) const
//{
//  _pContext->Unmap(texture.get_ptr(), 0);
//}

/* Currently not in use */
//void d3d_renderer::update_texture(const d3d_texture& texture, const char* pData, uint32_t subIndex) const
//{
//  D3D_CHECK_CALL(
//  _pContext->UpdateSubresource(texture.get_ptr(), subIndex, nullptr,
//    pData, texture.get_width() * texture.get_pixel_size(),
//    texture.get_width() * texture.get_height() * texture.get_pixel_size())
//  );
//}

void d3d_renderer::update_buffer(const d3d_buffer& buffer, const char* pData) const
{
  D3D_CHECK_CALL(
    _pContext->UpdateSubresource(buffer.get_ptr(), 0, nullptr,pData, 0, 0)
  );
}

void d3d_renderer::copy_buffer_region(const d3d_buffer& source, const d3d_buffer destination,
  uint32_t srcX, uint32_t srcY, uint32_t dstX, uint32_t dstY, uint32_t width, uint32_t height) const {
  D3D11_BOX srcBox = { 0 };
  srcBox.left = srcX;
  srcBox.top = srcY;
  srcBox.right = srcBox.left + width;
  srcBox.bottom = srcBox.top + height;
  _pContext->CopySubresourceRegion(destination.get_ptr(), 0, dstX, dstY, 0, source.get_ptr(), 0, &srcBox);
}

d3d_texture d3d_renderer::create_texture(uint32_t width, uint32_t height, const char* const* ppPixels, DXGI_FORMAT format, bool generateMips, access_mode access, texture_type type, uint32_t count, sampler_mode sampler) const
{
  return d3d_texture(width, height, ppPixels, _pDevice.Get(), _pContext.Get(), format, generateMips, access,type,count,sampler);
}

d3d_buffer d3d_renderer::create_buffer(const char* pData, uint32_t length, buffer_type type, uint32_t count, access_mode access) const
{
  return d3d_buffer(pData, length, type, _pDevice.Get(), count, access);
}

d3d_shader d3d_renderer::create_shader(const char* pBytes, size_t length) const
{
  return d3d_shader(pBytes, length, _pDevice.Get());
}

d3d_def_renderer d3d_renderer::create_def_renderer() const
{
  return d3d_def_renderer(_pDevice.Get(), _samplers, _blends, _quad, _renderTexture, _viewport);
}

void d3d_renderer::imgui_new_frame() const
{
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

void d3d_renderer::imgui_draw() const
{
  clear_state();

  ImGui::Render();

  constexpr float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  D3D_CHECK_CALL(
    _pContext->OMSetBlendState(_blends.get_blend_state(blend_mode::on), blendFactor, ~0u)
  );

  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool d3d_renderer::imgui_forward_msg(MSG& msg)
{
  return ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
}

void d3d_renderer::clear_state() const
{
  D3D_CHECK_CALL(
    _pContext->ClearState()
  );

  ID3D11RenderTargetView* pView = _renderTexture.get_render_view();
  D3D_CHECK_CALL(
    _pContext->OMSetRenderTargets(1, &pView, nullptr)
  );

  D3D_CHECK_CALL(
    _pContext->RSSetViewports(1, &_viewport);
  );
}

d3d_renderer::d3d_renderer(HWND hwnd)
{
  UINT flags = 0;

#ifdef _DEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
  D3D_FEATURE_LEVEL myFeatureLevel;

  RECT rect = { 0 };
  if (GetClientRect(hwnd, &rect) == false) {
    THROW_ERROR("Failed to get windows client rect.");
  }
  const auto width = (uint32_t)(rect.right - rect.left);  
  const auto height = (uint32_t)(rect.bottom - rect.top);

  DXGI_SWAP_CHAIN_DESC swapDesc = { 0 };
  swapDesc.Windowed = true;
  swapDesc.OutputWindow = hwnd;
  swapDesc.SampleDesc.Count = 1;
  swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  swapDesc.BufferCount = _bufferCount;
  swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapDesc.BufferDesc.Width = width;
  swapDesc.BufferDesc.Height = height;
  swapDesc.BufferDesc.RefreshRate.Numerator = 60;
  swapDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapDesc.BufferDesc.Format = outputFormat;
  swapDesc.Flags = _swapchainFlags;

  auto result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
    nullptr, flags, &featureLevel, 1u, D3D11_SDK_VERSION, &swapDesc, _pSwapchain.GetAddressOf(),
    _pDevice.GetAddressOf(), &myFeatureLevel, _pContext.GetAddressOf());
  if (result != S_OK) {
    THROW_GERROR("Failed to create device and swapchain");
  }

  D3D_CHECK_CALL(
    _pDevice->GetImmediateContext(_pContext.GetAddressOf())
  );

  D3D_CALL(
    _pSwapchain->GetBuffer(0, IID_PPV_ARGS(_pBackBuffer.GetAddressOf()))
  );

  _viewport.MaxDepth = 1;
  _viewport.Width = (float)width;
  _viewport.Height = (float)height;

  _renderTexture = create_texture(width, height, nullptr,
    outputFormat, false, access_mode::none, texture_type::render, 1, sampler_mode::point);

  clear_state();

  _samplers = std::move(d3d_sampler_col(_pDevice.Get()));
  _blends = std::move(d3d_blend_col(_pDevice.Get()));

  _quad = d3d_mesh::quad(_pDevice.Get());

  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(_pDevice.Get(), _pContext.Get());
}

void d3d_renderer::draw_quad(const d3d_material& material) const
{
  this->draw(material, _quad);
}

void d3d_renderer::draw_quad_instanced(const d3d_material& material, uint32_t count) const
{
  this->draw_instanced(material, _quad, count);
}
