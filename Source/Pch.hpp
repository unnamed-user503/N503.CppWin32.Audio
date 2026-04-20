#pragma once

// C++/STL
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Windows Dependencies
// 以下の定義はPremake5にて設定済み
// #define WIN32_LEAN_AND_MEAN
// #define NOMINMAX
// #define STRICT
// #define STRICT_TYPED_ITEMIDS
#include <Windows.h>     // 基本型 (WORD, DWORD) と Windows API 用
#include <mfapi.h>       // Media Foundation API
#include <mfidl.h>       // メディアプレゼンテーション関連インターフェース
#include <mfreadwrite.h> // Media Foundation の SourceReader 用
#include <mmreg.h>       // WAVEFORMATEX 構造体を含む

// Windows Implementation Libraries
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result.h>
// #include <wil/win32_helpers.h>
