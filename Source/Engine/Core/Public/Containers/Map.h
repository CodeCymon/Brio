// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "Containers/Array.h"
#include "Core/Hash.h"
#include "Core/CoreTypes.h"

template <typename KeyType, typename ValueType, typename HashFn = Hash<KeyType>>
class Map {
public:
    struct Entry {
        KeyType key;
        ValueType value;
    };
    
    using SizeType = typename Array<Entry>::SizeType;

private:
    Array<Array<Entry>> buckets;
    SizeType elementCount {};

    static constexpr SizeType kInitialBucketCount = 8;

public:
    [[nodiscard]] SizeType Size() const {
        return elementCount;
    }

    [[nodiscard]] bool IsEmpty() const {
        return elementCount == 0;
    }

    [[nodiscard]] ValueType* Get(KeyType const& key) {
        if (buckets.IsEmpty())
            return nullptr;
        for (Entry& entry : buckets[BucketIndexFor(key)])
            if (entry.key == key)
                return &entry.value;
        return nullptr;
    }

    [[nodiscard]] ValueType const* Get(KeyType const& key) const {
        if (buckets.IsEmpty())
            return nullptr;
        for (Entry const& entry : buckets[BucketIndexFor(key)])
            if (entry.key == key)
                return &entry.value;
        return nullptr;
    }

    [[nodiscard]] bool Contains(KeyType const& key) const {
        return Get(key) != nullptr;
    }

public:
    void Insert(KeyType const& key, ValueType const& value) {
        ValueType* pExisting = Get(key);
        if (pExisting == nullptr) {
            EnsureCapacityFor(elementCount + 1);
            buckets[BucketIndexFor(key)].Emplace(Entry{key, value});
            ++elementCount;
        } else {
            *pExisting = value;
        }
    }

    bool Remove(KeyType const& key) {
        if (buckets.IsEmpty())
            return false;
        Array<Entry>& bucket = buckets[BucketIndexFor(key)];
        for (SizeType i = 0; i < bucket.Size(); ++i) {
            if (bucket[i].key == key) {
                bucket.RemoveAt(i);
                --elementCount;
                return true;
            }
        }
        return false;
    }

    void Clear() {
        for (Array<Entry>& bucket : buckets)
            bucket.Clear();
        elementCount = 0;
    }

    void Reserve(SizeType expectedCount) {
        SizeType required = RequiredBucketCount(expectedCount);
        if (required > buckets.Size())
            Rehash(required);
    }

private:
    [[nodiscard]] SizeType BucketIndexFor(KeyType const& key) const {
        u64 hash = HashFn{}(key);
        return static_cast<SizeType>(hash & static_cast<u64>(buckets.Size() - 1));
    }

    [[nodiscard]] static SizeType RequiredBucketCount(SizeType expectedCount) {
        SizeType bucketsNeeded = kInitialBucketCount;
        while (expectedCount * 4 > bucketsNeeded * 3)
            bucketsNeeded *= 2;
        return bucketsNeeded;
    }

    void EnsureCapacityFor(SizeType newElementCount) {
        if (buckets.IsEmpty()) {
            buckets.Resize(kInitialBucketCount);
            return;
        }
        if (newElementCount * 4 > buckets.Size() * 3)
            Rehash(buckets.Size() * 2);
    }

    void Rehash(SizeType newBucketCount) {
        Array<Array<Entry>> newBuckets;
        newBuckets.Resize(newBucketCount);
        for (Array<Entry>& bucket : buckets) {
            for (Entry& entry : bucket) {
                u64 hash = HashFn{}(entry.key);
                auto idx = static_cast<SizeType>(hash & static_cast<u64>(newBucketCount - 1));
                newBuckets[idx].Add(Move(entry));
            }
        }

        buckets = Move(newBuckets);
    }

public:
    struct Iterator {
        Array<Array<Entry>>* buckets;
        SizeType bucketIndex;
        SizeType indexInBucket;

        void SkipEmptyBuckets() {
            while (bucketIndex < buckets->Size() && indexInBucket >= (*buckets)[bucketIndex].Size()) {
                ++bucketIndex;
                indexInBucket = 0;
            }
        }

        Entry& operator*() const { return (*buckets)[bucketIndex][indexInBucket]; }
        Entry* operator->() const { return &(*buckets)[bucketIndex][indexInBucket]; }

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
