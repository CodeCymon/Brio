// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Containers/Array.h"
#include "Core/CoreTypes.h"
#include "Core/Hash.h"

template<typename ValueType, typename HashFn = Hash<ValueType>>
class Set {
private:
    Array<Array<ValueType>> buckets;
    i32 elementCount {};

    static constexpr i32 kInitialBucketCount = 8;

public:
    [[nodiscard]] i32 Size() const {
        return elementCount;
    }

    [[nodiscard]] bool IsEmpty() const {
        return elementCount == 0;
    }

    [[nodiscard]] bool Contains(ValueType const& value) const {
        if (buckets.IsEmpty())
            return false;
        for (ValueType const& existing : buckets[BucketForIndex(value)])
            if (existing == value)
                return true;
        return false;
    }

public:
    void Add(ValueType const& value) {
        if (Contains(value))
            return;
        EnsureCapacityFor(elementCount + 1);
        buckets[BucketForIndex(value)].Add(value);
        ++elementCount;
    }

    bool Remove(ValueType const& value) {
        if (buckets.IsEmpty())
            return false;
        Array<ValueType>& bucket = buckets[BucketForIndex(value)];
        for (i32 i = 0; i < bucket.Size(); ++i) {
            if (bucket[i] == value) {
                bucket.RemoveAt(i);
                --elementCount;
                return true;
            }
        }
        return false;
    }

    void Clear() {
        for (Array<ValueType>& bucket : buckets)
            bucket.Clear();
        elementCount = 0;
    }

    void Reserve(i32 expectedCount) {
        i32 required = RequiredBucketCount(expectedCount);
        if (required > buckets.Size())
            Rehash(required);
    }

private:
    [[nodiscard]] i32 BucketForIndex(ValueType const& value) const {
        u64 hash = HashFn{}(value);
        return static_cast<i32>(hash & static_cast<u64>(buckets.Size() - 1));
    }

    [[nodiscard]] static i32 RequiredBucketCount(i32 expectedCount) {
        i32 bucketsNeeded = kInitialBucketCount;
        while (expectedCount * 4 > bucketsNeeded * 3)
            bucketsNeeded *= 2;
        return bucketsNeeded;
    }

    void EnsureCapacityFor(i32 newElementCount) {
        if (buckets.IsEmpty()) {
            buckets.Resize(kInitialBucketCount);
            return;
        }
        if (newElementCount * 4 > buckets.Size() * 3)
            Rehash(buckets.Size() * 2);
    }

    void Rehash(i32 newBucketCount) {
        Array<Array<ValueType>> newBuckets;
        newBuckets.Resize(newBucketCount);
        for (Array<ValueType>& bucket : buckets) {
            for (ValueType& value : bucket) {
                u64 hash = HashFn{}(value);
                i32 idx = static_cast<i32>(hash & static_cast<u64>(newBucketCount - 1));
                newBuckets[idx].Add(Move(value));
            }
        }
        buckets = Move(newBuckets);
    }

public:
    struct Iterator {
        Array<Array<ValueType>>* buckets;
        i32 bucketIndex;
        i32 indexInBucket;

        void SkipEmptyBuckets() {
            while (bucketIndex < buckets->Size() && indexInBucket >= (*buckets)[bucketIndex].Size()) {
                ++bucketIndex;
                indexInBucket = 0;
            }
        }

        ValueType& operator*() const { return (*buckets)[indexInBucket][indexInBucket]; }

        Iterator& operator++() {
            ++indexInBucket;
            SkipEmptyBuckets();
            return *this;
        }

        bool operator!=(Iterator const & other) const {
            return bucketIndex != other.bucketIndex || indexInBucket != other.indexInBucket;
        }
    };

    Iterator begin() {
        Iterator iter {&buckets, 0, 0};
        iter.SkipEmptyBuckets();
        return iter;
    }

    Iterator end() {
        return Iterator{&buckets, buckets.Size(), 0};
    }
};
