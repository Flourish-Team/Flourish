#pragma once

#include "Error/Error.h"

#include <functional>
#include <memory>

namespace Flourish
{
    typedef Error<std::shared_ptr<class DataStoreWriteStream>> DataStoreWriteCallbackParam;
    typedef std::function<void(DataStoreWriteCallbackParam)> DataStoreWriteCallback;
    typedef Error<std::shared_ptr<class DataStoreReadStream>> DataStoreReadCallbackParam;
    typedef std::function<void(DataStoreReadCallbackParam)> DataStoreReadCallback;
}
