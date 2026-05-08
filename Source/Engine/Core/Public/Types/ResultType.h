#pragma once

#include "Memory/MemoryUtils.h"

template <typename T>
struct TOk {
    T Value;
};

template <typename E>
struct TErr {
    E Error;
};

template<typename T, typename E>
class TResult {
private:
    bool bIsOk;

    union {
        T Value;
        E Error;
    };

public:
    explicit TResult(TOk<T> InOk) : bIsOk(true) {
        new (&Value) T(MoveTemp(InOk.Value));
    }

    explicit TResult(TErr<E> InErr) : bIsOk(false) {
        new (&Error) E(MoveTemp(InErr.Error));
    }

    ~TResult() {
        if (bIsOk)
            Value.~T();
        else
            Error.~E();
    }

    bool IsOk() const {
        return bIsOk;
    }

    bool IsErr() const {
        return !bIsOk;
    }

    T& Unwrap() {
        return Value;
    }

    T const& Unwrap() const {
        return Value;
    }

    E& UnwrapErr() {
        return Error;
    }

    E const& UnwrapErr() const {
        return Error;
    }
};


template <typename T>
TOk<T> Ok(T Value) {
    return TOk<T>{MoveTemp(Value)};
}

template <typename E>
TErr<E> Err(E Error) {
    return TErr<E>{MoveTemp(Error)};
}
