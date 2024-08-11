/***********************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 ****/

#include <objbase.h>
#include <stdio.h>
#include <displib_h.h>

int main(int argc, char** argv)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (SUCCEEDED(hr))
	{
		hr = CoRegisterPSClsid(IID_IHelloWorld, IID_IHelloWorld);

		if (SUCCEEDED(hr))
		{
			IUnknown* punk = NULL;

			hr = GetActiveObject(CLSID_CHelloWorld, NULL, &punk);

			if (SUCCEEDED(hr))
			{
				IHelloWorld* dispatch = NULL;

				hr = punk->QueryInterface(IID_IHelloWorld, (void**)&dispatch);

				if (SUCCEEDED(hr))
				{
					BSTR result = NULL;

					hr = dispatch->GetMessage(1, &result);

					if (SUCCEEDED(hr) && result)
					{
						printf("%S\n", result);

						SysFreeString(result);
					}

					dispatch->Release();
				}

				punk->Release();
			}
		}

		CoUninitialize();
	}

	if (FAILED(hr))
	{
		fprintf(stderr, "0x%lx\n", (long)hr);
	}

	return FAILED(hr);
}
