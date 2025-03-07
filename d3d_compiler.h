#pragma once
#pragma once

#include "d3d_inc.h"

namespace base {
	namespace graphics
	{
		class d3d_compiler final
		{
		public:
			enum class shader {
				undef,
				vertex,
				pixel
			};
		public:
			bool compile(const char* pBytes, size_t length, shader flag, ID3DBlob** ppBinary, ID3DBlob** ppError);
		};
	}
}