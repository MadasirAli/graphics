#include "d3d_compiler.h"

#include <string>
#include <assert.h>

using namespace base::graphics;

bool d3d_compiler::compile(const char* pBytes, size_t length, shader flag, ID3DBlob** ppBinary, ID3DBlob** ppError)
{
  UINT flags = 0;

#if _DEBUG
  flags |= D3DCOMPILE_DEBUG;
#else
  flags |= D3DCOMPILE_ENABLE_STRICTNESS;
#endif

  char entryPoint[8] = {0};
  char target[8] = { 0 };
  if (flag == shader::vertex) {
    strcpy_s(entryPoint, "vs_main");
    strcpy_s(target, "vs_5_0");
  }
  else if (flag == shader::pixel) {
    strcpy_s(entryPoint, "ps_main");
    strcpy_s(target, "ps_5_0");
  }
  else {
    assert(false);
  }

  auto result = D3DCompile(pBytes, length, nullptr, nullptr,
    D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, flags, 0, ppBinary, ppError);

  if (result == S_FALSE) {
    if (*ppError != nullptr) {
      std::string error = (char*)((*ppError)->GetBufferPointer());
      if (error.find("entry") != std::string::npos) {
        return false;
      }
      else {
        return true;
      }
    }
  }

  return true;
}