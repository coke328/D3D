#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <windows.h>

#include <wchar.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vector>
#include <comdef.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>
#include <directXTK/Mouse.h>
#include <directXTK/Keyboard.h>
#include <memory>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX::SimpleMath;
using namespace DirectX;