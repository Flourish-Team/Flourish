#pragma once

#include <functional>

#include "Error/Error.h"

namespace Flourish
{
    typedef std::function<void(Error<class DataStoreWriteStream*>)> DataStoreWriteCallback;
    typedef std::function<void(Error<class DataStoreReadStream*>)> DataStoreReadCallback;
}
