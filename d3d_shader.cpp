#include "d3d_shader.h"

#include "graphics_error.h"
#include "d3d_compiler.h"

using namespace base::graphics;

d3d_shader::d3d_shader(const char* bytes, size_t length, ID3D11Device* pDevice)
{
  d3d_compiler compiler{};

	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pVertexReflect{};
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pPixelReflect{};

  Microsoft::WRL::ComPtr<ID3DBlob> pPixel{};
	Microsoft::WRL::ComPtr<ID3DBlob> pVertex{};

  Microsoft::WRL::ComPtr<ID3DBlob> pError{};

  bool hasShader = compiler.compile(bytes, length, d3d_compiler::shader::vertex,
		pVertex.GetAddressOf(), pError.ReleaseAndGetAddressOf());

  if (hasShader) {
    if (pError.Get() != nullptr) {
      THROW_GERROR(std::string((char*)pError->GetBufferPointer()));
    }

    _flags |= (uint32_t)flags::vertex;
  }
  else {
    assert(false);
  }

  hasShader = compiler.compile(bytes, length, d3d_compiler::shader::pixel,
    pPixel.GetAddressOf(), pError.ReleaseAndGetAddressOf());

  if (hasShader) {
    if (pError.Get() != nullptr) {
      THROW_GERROR(std::string((char*)pError->GetBufferPointer()));
    }

    _flags |= (uint32_t)flags::pixel;
  }
  else {
    assert(false);
  }

  D3D_CALL(
    pDevice->CreatePixelShader(pPixel->GetBufferPointer(),
    pPixel->GetBufferSize(), nullptr, _pPixelShader.GetAddressOf())
	);

	D3D_CALL(
		D3DReflect(pPixel->GetBufferPointer(), pPixel->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)pPixelReflect.GetAddressOf())
	);

  D3D_CALL(
    pDevice->CreateVertexShader(pVertex->GetBufferPointer(),
			pVertex->GetBufferSize(), nullptr, _pVertexShader.GetAddressOf()))
    ;

  D3D_CALL(
    D3DReflect(pVertex->GetBufferPointer(), pVertex->GetBufferSize(),
      IID_ID3D11ShaderReflection, (void**)pVertexReflect.GetAddressOf())
  );

	reflect(pVertexReflect.Get(), pDevice, pVertex.Get(), _vertexReflect);
	reflect(pPixelReflect.Get(), nullptr, nullptr, _pixelReflect);
}

void d3d_shader::reflect(ID3D11ShaderReflection* pReflector, ID3D11Device* pDevice, ID3DBlob* pVertex,
	reflection& reflect_out)
{
	D3D11_SHADER_DESC desc = { 0 };
	auto result = pReflector->GetDesc(&desc);
	assert(result == S_OK);

	// reflecting bindable resources
	size_t textureCounter = 0;
	size_t samplerCounter = 0;
	size_t cBufferCounter = 0;
	size_t sBufferCounter = 0;
	for (auto i = 0u; i < desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC inputDesc = { 0 };
		result = pReflector->GetResourceBindingDesc(i, &inputDesc);
		assert(result == S_OK);

		assert(std::strlen(inputDesc.Name) < reflection::name_len);

		// for textures
		if (inputDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
		{
			assert(textureCounter < reflection::max_enties);

      strcpy_s(reflect_out.textures[textureCounter].first.data(), reflect_out.textures[textureCounter].first.size(), inputDesc.Name);

			reflect_out.textures[textureCounter].second = (uint16_t)inputDesc.BindPoint;

			++textureCounter;
		}
		// for samplers
		else if (inputDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
		{
			assert(samplerCounter < reflection::max_enties);
			strcpy_s(reflect_out.samplers[samplerCounter].first.data(), reflect_out.samplers[samplerCounter].first.size(), inputDesc.Name);

			reflect_out.samplers[samplerCounter].second = (uint16_t)inputDesc.BindPoint;

			++samplerCounter;
		}
		// for constant buffers
		else if (inputDesc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
		{
			assert(cBufferCounter < reflection::max_enties);
			strcpy_s(reflect_out.cBuffers[cBufferCounter].first.data(), reflect_out.cBuffers[cBufferCounter].first.size(), inputDesc.Name);

			reflect_out.cBuffers[cBufferCounter].second = (uint16_t)inputDesc.BindPoint;

			++cBufferCounter;
		}
		// for structured buffers
		else if (inputDesc.Type == D3D_SHADER_INPUT_TYPE::D3D11_SIT_STRUCTURED)
		{
			assert(sBufferCounter < reflection::max_enties);
			strcpy_s(reflect_out.sBuffers[sBufferCounter].first.data(), reflect_out.sBuffers[sBufferCounter].first.size(), inputDesc.Name);

			reflect_out.sBuffers[sBufferCounter].second = (uint16_t)inputDesc.BindPoint;

			++sBufferCounter;
		}
	}

	if (pVertex)
	{
		// reflecting and creating input layout
		std::array<D3D11_INPUT_ELEMENT_DESC, reflection::max_enties> inputs = { 0 };
		assert(desc.InputParameters < reflection::max_enties);

		UINT paramCount = 0;
		for (auto i = 0u; i < desc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc = { 0 };
			result = pReflector->GetInputParameterDesc(i, &paramDesc);
			assert(result == S_OK);

			if (paramDesc.SystemValueType != D3D_NAME_UNDEFINED) {
				continue;
			}

			// determine DXGI format
			DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputs[paramCount].SemanticName = paramDesc.SemanticName;
			inputs[paramCount].SemanticIndex = paramDesc.SemanticIndex;
			inputs[paramCount].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			inputs[paramCount].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
			inputs[paramCount].InputSlot = 0u;
			inputs[paramCount].InstanceDataStepRate = 0u;
			inputs[paramCount].Format = format;

			++paramCount;
		}

		D3D_CALL(
			pDevice->CreateInputLayout(inputs.data(), paramCount, pVertex->GetBufferPointer(),
				pVertex->GetBufferSize(), _pInputLayout.GetAddressOf())
		);
	}
}
