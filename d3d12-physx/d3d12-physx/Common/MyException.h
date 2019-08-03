#pragma once

#include <string>
#include <comdef.h>

#include "StringConvert.h"

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::wstring ToString()const {
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();
		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};

class MyException
{
public:
	MyException() = default;
	MyException(const std::string& err, const std::string& filename, int lineNumber) :
		Err(err),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::string ToString()const {
		return Err + ", failed in " + Filename + ", line " + std::to_string(LineNumber);
	}

	std::string Err;
	std::string Filename;
	int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ThrowMyEx
#define ThrowMyEx(x)										          \
{                                                                     \
    std::string fn = std::string(__FILE__);					          \
    throw MyException(x, fn, __LINE__);								  \
}
#endif