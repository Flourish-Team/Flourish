#pragma once

#include "Debug/Assert.h"

namespace Flourish
{
    template<typename T>
    class Error
    {
    public:
        static Error<T> Successful(const T& value)
        {
            return Error(value);
        }

        static Error<T> Failure(const char* error)
        {
            return Error(error);
        }

        bool HasError() const
        {
            return _error != nullptr;
        }

        T Value() const
        {
            FL_ASSERT_MSG(!HasError(), "Attempted to get value when there was an error: %s", _error);
            return _value;
        }

        const char* GetError() const
        {
            return _error;
        }

    private:
        Error(const T& value)
            : _value(value)
            , _error(nullptr)
        {
        }

        Error(const char* error)
            : _value(T())
            , _error(error)
        {
        }

        T _value;
        const char* _error;
    };
}
