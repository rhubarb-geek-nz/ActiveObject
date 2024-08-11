/***********************************
 * Copyright (c) 2024 Roger Brown.
 * Licensed under the MIT License.
 ****/

#include<windows.h>
#include <stdio.h>
#include <displib_h.h>

static HANDLE globalEvent;

typedef struct CHelloWorldData
{
	IUnknown IUnknown;
	IHelloWorld IHelloWorld;
	LONG lUsage;
	IUnknown* lpOuter;
	ITypeInfo* piTypeInfo;
} CHelloWorldData;

typedef struct CClassObjectData
{
	IClassFactory IClassFactory;
	LONG lUsage;
} CClassObjectData;

#define GetBaseObjectPtr(x,y,z)     (x *)(((char *)(void *)z)-(size_t)(char *)(&(((x*)NULL)->y)))

static STDMETHODIMP CHelloWorld_IUnknown_QueryInterface(IUnknown* pThis, REFIID riid, void** ppvObject)
{
	HRESULT hr = E_NOINTERFACE;
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IUnknown, pThis);

	if (IsEqualIID(riid, &IID_IDispatch) || IsEqualIID(riid, &IID_IHelloWorld))
	{
		IUnknown_AddRef(pData->lpOuter);

		*ppvObject = &(pData->IHelloWorld);

		hr = S_OK;
	}
	else
	{
		if (IsEqualIID(riid, &IID_IUnknown))
		{
			InterlockedIncrement(&pData->lUsage);

			*ppvObject = &(pData->IUnknown);

			hr = S_OK;
		}
		else
		{
			*ppvObject = NULL;
		}
	}

	return hr;
}

static STDMETHODIMP_(ULONG) CHelloWorld_IUnknown_AddRef(IUnknown* pThis)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IUnknown, pThis);
	return InterlockedIncrement(&pData->lUsage);
}

static STDMETHODIMP_(ULONG) CHelloWorld_IUnknown_Release(IUnknown* pThis)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IUnknown, pThis);
	LONG res = InterlockedDecrement(&pData->lUsage);

	if (!res)
	{
		if (pData->piTypeInfo) IUnknown_Release(pData->piTypeInfo);
		LocalFree(pData);
		if (0 == CoReleaseServerProcess())
		{
			HANDLE event = globalEvent;

			if (event)
			{
				SetEvent(event);
			}
		}
	}

	return res;
}

static STDMETHODIMP CHelloWorld_IHelloWorld_QueryInterface(IHelloWorld* pThis, REFIID riid, void** ppvObject)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IHelloWorld, pThis);
	return IUnknown_QueryInterface(pData->lpOuter, riid, ppvObject);
}

static STDMETHODIMP_(ULONG) CHelloWorld_IHelloWorld_AddRef(IHelloWorld* pThis)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IHelloWorld, pThis);
	return IUnknown_AddRef(pData->lpOuter);
}

static STDMETHODIMP_(ULONG) CHelloWorld_IHelloWorld_Release(IHelloWorld* pThis)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IHelloWorld, pThis);
	return IUnknown_Release(pData->lpOuter);
}

static STDMETHODIMP CHelloWorld_IHelloWorld_GetTypeInfoCount(IHelloWorld* pThis, UINT* pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

static STDMETHODIMP CHelloWorld_IHelloWorld_GetTypeInfo(IHelloWorld* pThis, UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IHelloWorld, pThis);
	if (iTInfo) return DISP_E_BADINDEX;
	ITypeInfo_AddRef(pData->piTypeInfo);
	*ppTInfo = pData->piTypeInfo;

	return S_OK;
}

static STDMETHODIMP CHelloWorld_IHelloWorld_GetIDsOfNames(IHelloWorld* pThis, REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IHelloWorld, pThis);

	if (!IsEqualIID(riid, &IID_NULL))
	{
		return DISP_E_UNKNOWNINTERFACE;
	}

	return DispGetIDsOfNames(pData->piTypeInfo, rgszNames, cNames, rgDispId);
}

static STDMETHODIMP CHelloWorld_IHelloWorld_Invoke(IHelloWorld* pThis, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	CHelloWorldData* pData = GetBaseObjectPtr(CHelloWorldData, IHelloWorld, pThis);

	if (!IsEqualIID(riid, &IID_NULL))
	{
		return DISP_E_UNKNOWNINTERFACE;
	}

	return DispInvoke(pThis, pData->piTypeInfo, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

static STDMETHODIMP CHelloWorld_IHelloWorld_GetMessage(IHelloWorld* pThis, int Hint, BSTR* lpMessage)
{
	*lpMessage = SysAllocString(Hint < 0 ? L"Goodbye, Cruel World" : L"Hello World");

	return S_OK;
}

static IUnknownVtbl CHelloWorld_IUnknownVtbl =
{
	CHelloWorld_IUnknown_QueryInterface,
	CHelloWorld_IUnknown_AddRef,
	CHelloWorld_IUnknown_Release
};

static IHelloWorldVtbl CHelloWorld_IHelloWorldVtbl =
{
	CHelloWorld_IHelloWorld_QueryInterface,
	CHelloWorld_IHelloWorld_AddRef,
	CHelloWorld_IHelloWorld_Release,
	CHelloWorld_IHelloWorld_GetTypeInfoCount,
	CHelloWorld_IHelloWorld_GetTypeInfo,
	CHelloWorld_IHelloWorld_GetIDsOfNames,
	CHelloWorld_IHelloWorld_Invoke,
	CHelloWorld_IHelloWorld_GetMessage
};

static STDMETHODIMP CreateInstance(const OLECHAR* typeLibFileName, IUnknown** ppvObject)
{
	ITypeLib* piTypeLib = NULL;
	HRESULT hr = LoadTypeLibEx(typeLibFileName, REGKIND_NONE, &piTypeLib);

	if (SUCCEEDED(hr))
	{
		CHelloWorldData* pData = LocalAlloc(LMEM_ZEROINIT, sizeof(*pData));

		if (pData)
		{
			IUnknown* p = &(pData->IUnknown);

			CoAddRefServerProcess();

			pData->IUnknown.lpVtbl = &CHelloWorld_IUnknownVtbl;
			pData->IHelloWorld.lpVtbl = &CHelloWorld_IHelloWorldVtbl;

			pData->lUsage = 1;
			pData->lpOuter = p;

			hr = ITypeLib_GetTypeInfoOfGuid(piTypeLib, &IID_IHelloWorld, &pData->piTypeInfo);

			if (SUCCEEDED(hr))
			{
				*ppvObject = p;
			}
			else
			{
				IUnknown_Release(p);
			}
		}

		if (piTypeLib)
		{
			ITypeLib_Release(piTypeLib);
		}
	}

	return hr;
}

static BOOL WINAPI ctrlHandler(DWORD dw)
{
	return SetEvent(globalEvent);
}

int main(int argc, char** argv)
{
	HRESULT hr;
	OLECHAR typeLibFileName[260];
	DWORD len = GetModuleFileNameW(GetModuleHandle(NULL), typeLibFileName, sizeof(typeLibFileName) / sizeof(typeLibFileName[0]));

	while (len--)
	{
		if (typeLibFileName[len] == '\\')
		{
			wcscpy_s(typeLibFileName + 1 + len, (sizeof(typeLibFileName) / sizeof(typeLibFileName[0])) - len - 1, L"displib.dll");
			break;
		}
	}

	globalEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	SetConsoleCtrlHandler(ctrlHandler, TRUE);

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		hr = CoRegisterPSClsid(&IID_IHelloWorld, &IID_IHelloWorld);

		if (SUCCEEDED(hr))
		{
			IUnknown* punk = NULL;

			hr = CreateInstance(typeLibFileName, &punk);

			if (SUCCEEDED(hr))
			{
				DWORD dwRegister = 0;

				hr = RegisterActiveObject(punk, &CLSID_CHelloWorld, ACTIVEOBJECT_STRONG, &dwRegister);

				if (SUCCEEDED(hr))
				{
					__try
					{
						WaitForSingleObject(globalEvent, INFINITE);
					}
					__finally
					{
						hr = RevokeActiveObject(dwRegister, NULL);
					}
				}

				IUnknown_Release(punk);
			}
		}

		CoUninitialize();
	}

	SetConsoleCtrlHandler(ctrlHandler, FALSE);

	{
		HANDLE event = globalEvent;
		globalEvent = NULL;
		CloseHandle(event);
	}

	if (FAILED(hr))
	{
		fprintf(stderr, "0x%lx\n", (long)hr);
	}

	return FAILED(hr);
}
