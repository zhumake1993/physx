#include "D3D12App.h"

int main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	HINSTANCE hMyInstance = GetModuleHandle(NULL);

	try
	{
		D3D12App theApp(hMyInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		Log(WStringToString(e.ToString()));
		return 0;
	}
	catch (MyException& e)
	{
		Log(e.ToString().c_str());
		return 0;
	}
	catch (MyPxException& e)
	{
		Log(e.ToString().c_str());
		return 0;
	}

	return 0;
}