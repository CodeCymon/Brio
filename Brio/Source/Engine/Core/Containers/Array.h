#pragma once
#include <optional>

#include "Core/Asserts/Assert.h"

template<typename T>
class BAPI TArray {
private:
    T* data_ { nullptr };
    usize size_ { 0 };
    usize capacity_ { 0 };

    void grow(usize newCapacity) {
        T* newData = new T[newCapacity];
        for (usize i = 0; i < size_; ++i) {
            newData[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
    }
    void  ensureCapacity() {
        if (size_ >= capacity_) {
            grow(capacity_ == 0 ? 4 : capacity_ * 2);
        }
    }

public:
    TArray() = default;
    explicit TArray(usize size) {
        if (size != 0) {
            grow(size);
            size_ = size;
        }

    }

    TArray(std::initializer_list<T> list) {
        reserve(list.size());
        for (T const& elem : list) {
            push_back(elem);
        }
    }

    TArray(T* first, T* last) {
        const i64 count = (last - first);
        ASSERT(count > 0, "Array: Cant construct empty Array from pointers!");

        grow(count);
        for (usize i = 0; i < count; ++i) {
            data_[i] = std::move(first[i]);
        }
        size_ = count;
    }

    TArray(TArray const& other) {
        if (other.size_ != 0) {
            grow(other.size_);
            for (usize i = 0; i < other.size_; ++i) {
                data_[i] = other.data_[i];
            }
            size_ = other.size_;
        }
    }

    TArray(TArray&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    TArray& operator=(TArray const& other) {
        if (this == &other)
            return *this;
        TArray temp(other);
        std::swap(data_, temp.data_);
        std::swap(size_, temp.size_);
        std::swap(capacity_, temp.capacity_);
        return *this;
    }

    TArray& operator=(TArray&& other) noexcept {
        if (this == &other)
            return *this;

        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    ~TArray() {
        delete[] data_;
    }

    [[nodiscard]] usize size() const { return size_; }
    [[nodiscard]] usize capacity() const { return capacity_; }

    T* data() {
        return data_;
    }

    T const* data() const {
        return data_;
    }

    T& operator[](usize index) {
        ASSERT(index < size_, "Array: Index out of bounds!");
        return data_[index];
    }

    T const& operator[](usize index) const {
        ASSERT(index < size_, "Array: Index out of bounds!");
        return data_[index];
    }

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    void push_back(T const& elem) {
        ensureCapacity();
        data_[size_++] = elem;
    }

    void push_back(T&& elem) {
        ensureCapacity();
        data_[size_++] = std::move(elem);
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) {
        ensureCapacity();
        data_[size_] = T(std::forward<Args>(args)...);
        return data_[size_++];
    }

    void pop_back() {
        ASSERT(size_ != 0, "Array: cant pop empty array!");
        --size_;
    }

    void insert(usize index, T const& elem) {
        ASSERT(index <= size_, "Array: Insert out of bounds!");
        ensureCapacity();
        for (usize i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = elem;
        ++size_;
    }

    void erase(usize index) {
        ASSERT(index < size_, "Array: Erase out of bounds!");
        for (usize i = index; i + 1 < size_; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
    }

    void clear() {
        size_ = 0;
    }

    void reserve(usize capacity) {
        if (capacity > capacity_)
            grow(capacity);
    }

    void resize(usize newSize) {
        if (newSize > capacity_) {
            grow(newSize);
        }
        size_ = newSize;
    }

    using Iterator = T*;
    using ConstIterator = T const*;

    Iterator begin() { return &data_[0]; }
    Iterator end() { return &data_[size()]; }
    ConstIterator begin() const { return &data_[0]; }
    ConstIterator end() const { return &data_[size()]; }
};



template<typename T, usize N>
class BAPI TStaticArray {
public:
    T data_[N];

    using Iterator = T*;
    using ConstIterator = T const*;

    [[nodiscard]] constexpr usize size() const {
        return N;
    }

    T* data() {
        return data_;
    }

    T const* data() const {
        return data_;
    }

    T& operator[](usize index) {
        ASSERT(index < N, "StaticArray: Index out of bounds!");
        return data_[index];
    }

    T const& operator[](usize index) const {
        ASSERT(index < N, "StaticArray: Index out of bounds!");
        return data_[index];
    }

    Iterator begin() { return &data_[0]; }
    Iterator end() { return &data_[size()]; }
    ConstIterator begin() const { return &data_[0]; }
    ConstIterator end() const { return &data_[size()]; }
};

template<typename T, typename... U>
TStaticArray(T, U...) -> TStaticArray<T, 1 + sizeof...(U)>;
