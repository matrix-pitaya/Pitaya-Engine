#include<Core/Identifier/GUID.h>

#include<cstring>

#include<objbase.h>

Pitaya::Core::GUID Pitaya::Core::GUID::New()
{
	Pitaya::Core::GUID result;
    ::GUID winGuid;
    if (::CoCreateGuid(&winGuid) == S_OK)
    {
        std::memcpy(&result, &winGuid, sizeof(GUID));
    }
    return result;
}
