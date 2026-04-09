#pragma once
#include <optional>

#include "Core/Asserts/Assert.h"

template<typename T>
class TArray;

// NOTE: inlining this interface may be beneficial in the future...

template<typename T>
class IArray {
public:
    virtual ~IArray() = default;

    [[nodiscard]] virtual usize size() const = 0;
    virtual T* data() = 0;
    virtual T const* data() const = 0;
    virtual T& operator[](usize index) = 0;
    virtual T const& operator[](usize index) const = 0;
    [[nodiscard]] virtual bool empty() const { return size() == 0; }

    template<typename U, typename Fn>
    TArray<U> map(Fn&& fn) const {
        TArray<U> result;
        result.reserve(size());
        for (usize i = 0; i < size(); ++i) {
            result.pushBack(fn((*this)[i]));
        }
        return result;
    }

    template<typename Fn>
    TArray<T> filter(Fn&& predicate) const {
        TArray<T> result;
        for (usize i = 0; i < size(); ++i) {
            if (predicate((*this)[i]))
                result.pushBack((*this)[i]);
        }
        return result;
    }

    template<typename Fn>
    std::optional<T> find_if(Fn&& predicate) const {
        for (usize i = 0; i < size(); ++i) {
            if (predicate((*this)[i]))
                return (*this)[i];
        }
        return std::nullopt;
    }

    std::optional<T> find(T const& value) const {
        for (usize i = 0; i < size(); ++i) {
            if (*this[i] == value)
                return (*this)[i];
        }
        return std::nullopt;
    }

    template<typename Fn>
    usize find_index_if(Fn&& predicate) const {
        for (usize i = 0; i < size(); ++i) {
            if (predicate((*this)[i]))
                return (i);
        }
        return (size());
    }

    usize find_index(T const& value) const {
        for (usize i = 0; i < size(); ++i) {
            if ((*this)[i] == value)
                return (i);
        }
        return (size());
    }

    template<typename Fn>
    bool contains_if(Fn&& predicate) const {
        return find_index_if(predicate) != size();
    }

    bool contains(T const& value) const {
        return find_index(value) != size();
    }

    template<typename Fn>
    bool any(Fn&& predicate) const {
        return contains_if(predicate);
    }

    template<typename Fn>
    bool all(Fn&& predicate) const {
        for (usize i = 0; i < size(); ++i) {
            if (!predicate((*this)[i]))
                return false;
        }
        return true;
    }

    template<typename Fn>
    bool none(Fn&& predicate) const {
        return !any(predicate);
    }


    void reverse() {
        for (usize lo = 0, hi = size(); hi > 0 && lo < hi; --hi, ++lo) {
            std::swap((*this)[lo], (*this)[hi]);
        }
    }

    // std::vector<T> toStdVector() const {
    //     return std::vector<T>(begin(), end());
    // }

    struct Iterator {
        IArray<T>* array;
        usize index;
        T& operator*() { return (*array)[index]; }
        bool operator!=(Iterator const& other) const { return index != other.index; }
        Iterator& operator++() { ++index; return *this; }
    };
    struct ConstIterator {
        IArray<T> const* array;
        usize index;
        T const& operator*() const { return (*array)[index]; }
        bool operator!=(ConstIterator const& other) const { return index != other.index; }
        ConstIterator& operator++() { ++index; return *this; }
    };
    Iterator begin() { return {this, 0}; }
    Iterator end() { return {this, size()}; }
    ConstIterator begin() const { return {this, 0}; }
    ConstIterator end() const { return {this, size()}; }
};



template<typename T>
class BAPI TArray final : public IArray<T> {
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

    ~TArray() override {
        delete[] data_;
    }

    [[nodiscard]] usize size() const override { return size_; }
    [[nodiscard]] usize capacity() const { return capacity_; }

    T* data() override {
        return data_;
    }

    T const* data() const override {
        return data_;
    }

    T& operator[](usize index) override {
        ASSERT(index < size_, "Array: Index out of bounds!");
        return data_[index];
    }

    T const& operator[](usize index) const override {
        ASSERT(index < size_, "Array: Index out of bounds!");
        return data_[index];
    }

    [[nodiscard]] bool empty() const override {
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
};



template<typename T, usize N>
class BAPI TStaticArray final : public IArray<T> {
private:
    T data_[N];

public:
    TStaticArray() = default;

    TStaticArray(std::initializer_list<T> list) {
        for (T const& elem : list) {
            push_back(elem);
        }
    }

    [[nodiscard]] constexpr usize size() const override {
        return N;
    }

    [[nodiscard]] constexpr usize capacity() const {
        return N;
    }

    T* data() override {
        return data_;
    }

    T const* data() const override {
        return data_;
    }

    T& operator[](usize index) override {
        ASSERT(index < N, "StaticArray: Index out of bounds!");
        return data_[index];
    }

    T const& operator[](usize index) const override {
        ASSERT(index < N, "StaticArray: Index out of bounds!");
        return data_[index];
    }
};
