#pragma once

// メモリ解放
#define SafeRelease(x) if(x){ x->Release(); x = nullptr; }
#define SafeDelete(x)  if(x){ delete x;  x = nullptr; }
#define SafeDeleteArray(x)  if(x){ delete[] x;  x = nullptr; }
#define SafeFree(x)  if(x){ free( x );  x = nullptr; }

// インクルード
#include <assert.h>
#include <windows.h>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include <map>
#include <list>
#include <array>
#include <memory>
#include <functional>
#include <utility>

using namespace std;


inline int Sum(vector<int>& Src)
{
	return accumulate(Src.begin(), Src.end(), 0);
}