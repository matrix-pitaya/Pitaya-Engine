#include<Core/Utils/Memory/Memory.h>

#include<mimalloc.h>

namespace
{
	void MimallocPrintFilter(const char* message, void* arg)
	{
		if (message == nullptr || arg == nullptr) { return; }
		std::string* str = static_cast<std::string*>(arg);
		str->append(message);
	}
}

std::string Pitaya::Core::GetMemoryState()
{ 
	std::string message;
	mi_stats_print_out(::MimallocPrintFilter, &message);
	return message;
}