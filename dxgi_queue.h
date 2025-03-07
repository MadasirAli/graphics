#pragma once

#include <string>

#include "d3d_inc.h"

namespace base {
#ifdef _DEBUG

	class dxgi_queue final
	{
	private:
		unsigned long get_message_count() const;

	public:
		void set_info_queue();
		std::string get_messages();
		bool check() const;

		dxgi_queue();
		~dxgi_queue() noexcept;

		dxgi_queue(const dxgi_queue&) = delete;
		dxgi_queue& operator==(const dxgi_queue&) = delete;

		dxgi_queue(dxgi_queue&&) noexcept = delete;
		dxgi_queue& operator==(dxgi_queue&&) noexcept = delete;

		static dxgi_queue& get_instance();

	private:
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> _pQueue = nullptr;
		HMODULE _libHandle;
		unsigned long _messageIndex;
	};
#endif
}