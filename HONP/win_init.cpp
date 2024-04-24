#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif


#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>


int main() {
	WSADATA data;

	if (WSAStartup(MAKEWORD(2, 2), &data))
	{
		std::cout << "Failed to initialize."<< std::endl;
		return -1;
	}

	DWORD asize = 20000;
	PIP_ADAPTER_ADDRESSES adapters;
	do {
		adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);
		if (!adapters)
		{
			std::cout << "Couldn't allocate " << asize << " bytes for adapters" << std::endl;
			WSACleanup();
			return -1;
		}

		int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize);

		if (r == ERROR_BUFFER_OVERFLOW) 
		{
			std::cout << "GetAdapterAddresses wants " << asize << " bites" << std::endl;
			free(adapters);
		}
		else if (r == ERROR_SUCCESS)
		{
			break;
		}
		else
		{
			std::cout << "Error from GetAdapterAddresses:" << r << std::endl;
			free(adapters);
			WSACleanup();
			return -1;
		}
	} while (!adapters);


	PIP_ADAPTER_ADDRESSES adapter = adapters;
	while (adapter)
	{
		printf("\nAdapter name: %S\n", adapter->FriendlyName);
		PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
		while (address)
		{
			printf("\t%s",
				address->Address.lpSockaddr->sa_family == AF_INET ?
				"IPv4" : "IPv6");
			char ap[100];
			getnameinfo(address->Address.lpSockaddr,
				address->Address.iSockaddrLength,
				ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
			printf("\t%s\n", ap);
			address = address->Next;
		}
		adapter = adapter->Next;
	}
	

	WSACleanup();
	std::cout << "Ok"<<std::endl;
	return 0;
}