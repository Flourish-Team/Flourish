#pragma once

#include <functional>

#include "Error/Error.h"

namespace Flourish
{
    typedef Error<class DataStoreWriteStream*> DataStoreWriteCallbackParam;
    typedef std::function<void(DataStoreWriteCallbackParam)> DataStoreWriteCallback;
    typedef Error<class DataStoreReadStream*> DataStoreReadCallbackParam;
    typedef std::function<void(DataStoreReadCallbackParam)> DataStoreReadCallback;
}
