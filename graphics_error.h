#pragma once

#include "general_error.h"
#include "dxgi_queue.h"

namespace base {

#define THROW_GERROR(msg)																\
{																						\
	throw graphics_error(																\
			std::string(msg) + std::string("\n\nFile: ") + std::string(__FILE__) + std::string("\nLine: ") + std::to_string(__LINE__));	\
}																						\

#ifdef _DEBUG

#define D3D_CALL(__func)																\
{																						\
	auto& __queue = dxgi_queue::get_instance();										\
    __queue.set_info_queue();																\
	const auto __result = __func;														\
	std::string __msg;																	\
	if (__result != S_OK || __queue.check())											\
	{																					\
		__msg += __queue.get_messages();													\
		THROW_GERROR(__msg);															\
	}																					\
}																						\

#define D3D_CHECK_CALL(__func)															\
{																						\
	auto& __queue = dxgi_queue::get_instance();										\
	__queue.set_info_queue();																\
	__func;																				\
	if (__queue.check())																\
	{																					\
		std::string __msg = __queue.get_messages();										\
		THROW_GERROR(__msg);															\
	}																					\
}																						\

#else

#define D3D_CALL(func)																	\
{																						\
	const auto __result = func;															\
	std::string __msg;																	\
	if (__result != S_OK)																\
	{																					\
		THROW_GERROR(__msg);															\
	}																					\
}																						\

#define D3D_CHECK_CALL(func)					\
{												\
	func;										\
}												\

#endif


  class graphics_error : public general_error
  {
  public:
    graphics_error(const std::string& msg) : general_error(msg) {}

    virtual ~graphics_error() noexcept override = default;
  };
}