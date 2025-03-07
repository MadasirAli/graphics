#ifdef _DEBUG

#include "dxgi_queue.h"

#include <memory>
#include <sstream>

#include "general_error.h"

using namespace base;

bool dxgi_queue::check() const
{
	return _messageIndex != get_message_count();
}

void dxgi_queue::set_info_queue()
{
	_messageIndex = get_message_count();
}

unsigned long dxgi_queue::get_message_count() const
{
	const auto count = _pQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	return (unsigned long)count;
}

std::string dxgi_queue::get_messages()
{
	const unsigned long startIndex = _messageIndex;
	set_info_queue();

	const unsigned long endIndex = _messageIndex;

	std::stringstream stream;

	stream << "[DXGI Info Queue]\n\n";

	for (auto i = startIndex; i < endIndex; i++)
	{
		unsigned long long bufferSize = 0;

		_pQueue->GetMessageW(DXGI_DEBUG_ALL,
			i, nullptr, &bufferSize);

		const auto pBuffer = std::make_unique<char[]>(bufferSize);

		_pQueue->GetMessageW(DXGI_DEBUG_ALL,
			i, (DXGI_INFO_QUEUE_MESSAGE*)pBuffer.get(), &bufferSize);

		stream << ((DXGI_INFO_QUEUE_MESSAGE*)pBuffer.get())->pDescription;
		stream << std::endl;
	}

	return stream.str();
}

dxgi_queue& dxgi_queue::get_instance()
{
	static dxgi_queue instance{};
	return instance;
}

dxgi_queue::dxgi_queue()
{
	const auto handle = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (handle == nullptr)
		THROW_LAST_WIN32_ERROR("Failed to load dxgidebug.dll.");

	_libHandle = handle;

	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
	const auto funPtr = reinterpret_cast<DXGIGetDebugInterface>(
		GetProcAddress(_libHandle, "DXGIGetDebugInterface"));

	if (funPtr == nullptr)
		THROW_LAST_WIN32_ERROR("Failed to obtain dxgo debug interface.");

	WIN32_CALL(
		funPtr(__uuidof(IDXGIInfoQueue), (void**)_pQueue.ReleaseAndGetAddressOf()));

	_messageIndex = 0;
}

dxgi_queue::~dxgi_queue() noexcept
{
	if (_libHandle != nullptr)
		FreeLibrary(_libHandle);
}

#endif