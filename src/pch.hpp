#pragma once

#include <math.h>    // IWYU pragma: keep
#include <cinttypes> // IWYU pragma: keep


#include <set>           // IWYU pragma: export
#include <map>           // IWYU pragma: export
#include <array>         // IWYU pragma: export
#include <bitset>        // IWYU pragma: export
#include <vector>        // IWYU pragma: export
#include <unordered_set> // IWYU pragma: export
#include <unordered_map> // IWYU pragma: export

#include <memory>        // IWYU pragma: export
#include <concepts>      // IWYU pragma: export
#include <coroutine>     // IWYU pragma: export
#include <algorithm>     // IWYU pragma: export
#include <functional>    // IWYU pragma: export

#include <frozen/set.h>           // IWYU pragma: export
#include <frozen/map.h>           // IWYU pragma: export
#include <frozen/string.h>        // IWYU pragma: export
#include <frozen/random.h>        // IWYU pragma: export
#include <frozen/algorithm.h>     // IWYU pragma: export
#include <frozen/unordered_set.h> // IWYU pragma: export
#include <frozen/unordered_map.h> // IWYU pragma: export

using c8  = char8_t;
using c16 = char16_t;
using c32 = char32_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

#include <QDebug>
#include <QString>
#include <QOverload>
#include <QStringLiteral>

using namespace Qt::StringLiterals;

#include "Ref.hpp"   // IWYU pragma: export
#include "Scope.hpp" // IWYU pragma: export
#include "Local.hpp" // IWYU pragma: export
#include "Enum.hpp"  // IWYU pragma: export

#include "Log.hpp"   // IWYU pragma: export
