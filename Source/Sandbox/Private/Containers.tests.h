// Copyright (c) Simon Kirsch 2026.
//
// Container tests for:
//   - Hash
//   - Set
//   - Map
//   - Array
//   - FixedArray
//   - Queue
//
// These tests intentionally use ASSERT(...) for correctness checks and
// LOG_DEBUG(LogTest, "...", ...) for observable test output.

#pragma once

#include "Containers/Array.h"
#include "Containers/FixedArray.h"
#include "Containers/Map.h"
#include "Containers/Queue.h"
#include "Containers/Set.h"

#include "Core/CoreTypes.h"
#include "Core/Hash.h"
#include "Log/Assert.h"
#include "Log/Log.h"

// ============================================================================
// Test helper types
// ============================================================================

struct TestHashKey {
    i32 id {};
    i32 category {};

    bool operator==(TestHashKey const& other) const {
        return id == other.id && category == other.category;
    }
};

// This demonstrates how an arbitrary type can be made hashable with the
// current Hash<T> design.
//
// IMPORTANT:
// This specialization must be visible before Set<TestHashKey> or
// Map<TestHashKey, ...> is instantiated.
template<>
struct Hash<TestHashKey> {
    u64 operator()(TestHashKey const& key) const noexcept {
        // Combine the two fields before mixing them.
        u64 combined =
            (static_cast<u64>(static_cast<u32>(key.id)) << 32)
            | static_cast<u32>(key.category);

        return MixBits(combined);
    }
};


// A deliberately terrible hash function.
//
// This is useful because Set/Map should still work correctly when every
// key hashes to exactly the same bucket.
struct ConstantHash {
    template<typename T>
    u64 operator()(T const&) const noexcept {
        return 0;
    }
};


// A slightly more realistic custom key with a custom hash.
struct TestUser {
    i32 id {};
    i32 age {};
    i32 score {};

    bool operator==(TestUser const& other) const {
        return id == other.id
            && age == other.age
            && score == other.score;
    }
};

template<>
struct Hash<TestUser> {
    u64 operator()(TestUser const& user) const noexcept {
        u64 hash = MixBits(static_cast<u64>(static_cast<u32>(user.id)));
        hash ^= MixBits(static_cast<u64>(static_cast<u32>(user.age)));
        hash ^= MixBits(static_cast<u64>(static_cast<u32>(user.score)));

        return MixBits(hash);
    }
};


// A type that lets us test non-trivial object lifetime/movement.
//
// The counters aren't strictly required for the container correctness
// tests, but they make lifetime problems much easier to spot while running
// under a debugger/sanitizer.
struct TestObject {
    i32 value {};

    static inline i32 constructed {};
    static inline i32 destroyed {};

    TestObject() {
        ++constructed;
    }

    explicit TestObject(i32 v)
    : value(v) {
        ++constructed;
    }

    TestObject(TestObject const& other)
    : value(other.value) {
        ++constructed;
    }

    TestObject(TestObject&& other) noexcept
    : value(other.value) {
        ++constructed;
    }

    TestObject& operator=(TestObject const& other) {
        value = other.value;
        return *this;
    }

    TestObject& operator=(TestObject&& other) noexcept {
        value = other.value;
        return *this;
    }

    ~TestObject() {
        ++destroyed;
    }

    bool operator==(TestObject const& other) const {
        return value == other.value;
    }

    static void ResetCounters() {
        constructed = 0;
        destroyed = 0;
    }
};


// ============================================================================
// Hash tests
// ============================================================================

void Test_Hash_MixBits() {
    LOG_DEBUG(LogTest, "Testing MixBits...");

    u64 zero = MixBits(0);
    u64 one = MixBits(1);
    u64 two = MixBits(2);
    u64 max = MixBits(~u64(0));

    LOG_DEBUG(LogTest, "MixBits(0) = {}", zero);
    LOG_DEBUG(LogTest, "MixBits(1) = {}", one);
    LOG_DEBUG(LogTest, "MixBits(2) = {}", two);
    LOG_DEBUG(LogTest, "MixBits(UINT64_MAX) = {}", max);

    // Determinism is an important property.
    ASSERT(MixBits(0) == zero);
    ASSERT(MixBits(1) == one);
    ASSERT(MixBits(2) == two);
    ASSERT(MixBits(~u64(0)) == max);

    // We intentionally DO NOT assert that these hashes are different.
    //
    // A hash function is allowed to collide. What matters is that the
    // containers handle collisions correctly.
    LOG_DEBUG(
        LogTest,
        "MixBits determinism verified; collisions are intentionally not treated as impossible"
    );
}


void Test_Hash_AllTrivialIntegerTypes() {
    LOG_DEBUG(LogTest, "Testing Hash<T> specializations for integer types...");

    Hash<bool> hashBool;
    Hash<char> hashChar;
    Hash<signed char> hashSignedChar;
    Hash<unsigned char> hashUnsignedChar;
    Hash<short> hashShort;
    Hash<unsigned short> hashUnsignedShort;
    Hash<int> hashInt;
    Hash<unsigned int> hashUnsignedInt;
    Hash<long> hashLong;
    Hash<unsigned long> hashUnsignedLong;
    Hash<unsigned long long> hashUnsignedLongLong;

    u64 boolHash = hashBool(true);
    u64 charHash = hashChar('A');
    u64 signedCharHash = hashSignedChar(static_cast<signed char>(-42));
    u64 unsignedCharHash = hashUnsignedChar(static_cast<unsigned char>(42));
    u64 shortHash = hashShort(static_cast<short>(-1234));
    u64 unsignedShortHash = hashUnsignedShort(static_cast<unsigned short>(1234));
    u64 intHash = hashInt(-123456);
    u64 unsignedIntHash = hashUnsignedInt(123456u);
    u64 longHash = hashLong(static_cast<long>(-123456789));
    u64 unsignedLongHash = hashUnsignedLong(static_cast<unsigned long>(123456789));
    u64 unsignedLongLongHash =
        hashUnsignedLongLong(0x123456789ABCDEF0ULL);

    LOG_DEBUG(LogTest, "Hash<bool>(true) = {}", boolHash);
    LOG_DEBUG(LogTest, "Hash<char>('A') = {}", charHash);
    LOG_DEBUG(LogTest, "Hash<signed char>(-42) = {}", signedCharHash);
    LOG_DEBUG(LogTest, "Hash<unsigned char>(42) = {}", unsignedCharHash);
    LOG_DEBUG(LogTest, "Hash<short>(-1234) = {}", shortHash);
    LOG_DEBUG(LogTest, "Hash<unsigned short>(1234) = {}", unsignedShortHash);
    LOG_DEBUG(LogTest, "Hash<int>(-123456) = {}", intHash);
    LOG_DEBUG(LogTest, "Hash<unsigned int>(123456) = {}", unsignedIntHash);
    LOG_DEBUG(LogTest, "Hash<long>(-123456789) = {}", longHash);
    LOG_DEBUG(LogTest, "Hash<unsigned long>(123456789) = {}", unsignedLongHash);
    LOG_DEBUG(LogTest, "Hash<unsigned long long>(...) = {}", unsignedLongLongHash);

    // Verify determinism for each specialization.
    ASSERT(hashBool(true) == boolHash);
    ASSERT(hashChar('A') == charHash);
    ASSERT(hashSignedChar(static_cast<signed char>(-42)) == signedCharHash);
    ASSERT(hashUnsignedChar(static_cast<unsigned char>(42)) == unsignedCharHash);
    ASSERT(hashShort(static_cast<short>(-1234)) == shortHash);
    ASSERT(hashUnsignedShort(static_cast<unsigned short>(1234)) == unsignedShortHash);
    ASSERT(hashInt(-123456) == intHash);
    ASSERT(hashUnsignedInt(123456u) == unsignedIntHash);
    ASSERT(hashLong(static_cast<long>(-123456789)) == longHash);
    ASSERT(hashUnsignedLong(static_cast<unsigned long>(123456789)) == unsignedLongHash);
    ASSERT(hashUnsignedLongLong(0x123456789ABCDEF0ULL) == unsignedLongLongHash);
}


void Test_Hash_IntegerEdgeCases() {
    LOG_DEBUG(LogTest, "Testing integer hash edge cases...");

    Hash<int> hashInt;
    Hash<unsigned int> hashUnsignedInt;

    int signedValues[] = {
        0,
        1,
        -1,
        2,
        -2,
        127,
        -127,
        128,
        -128,
        255,
        -255,
        1024,
        -1024,
    };

    for (int value : signedValues) {
        u64 hash = hashInt(value);

        LOG_DEBUG(
            LogTest,
            "Hash<int> value {} -> {}",
            value,
            hash
        );

        // Calling it again must produce exactly the same result.
        ASSERT(hashInt(value) == hash);
    }

    unsigned int unsignedValues[] = {
        0u,
        1u,
        2u,
        255u,
        256u,
        1024u,
        0x7FFFFFFFu,
        0xFFFFFFFFu,
    };

    for (unsigned int value : unsignedValues) {
        u64 hash = hashUnsignedInt(value);

        LOG_DEBUG(
            LogTest,
            "Hash<unsigned int> value {} -> {}",
            value,
            hash
        );

        ASSERT(hashUnsignedInt(value) == hash);
    }
}


void Test_Hash_Pointers() {
    LOG_DEBUG(LogTest, "Testing Hash<T*>...");

    int first = 123;
    int second = 456;

    Hash<int*> hashPointer;

    u64 firstHash = hashPointer(&first);
    u64 secondHash = hashPointer(&second);

    LOG_DEBUG(LogTest, "Address of first = {}", static_cast<void*>(&first));
    LOG_DEBUG(LogTest, "Hash of first pointer = {}", firstHash);

    LOG_DEBUG(LogTest, "Address of second = {}", static_cast<void*>(&second));
    LOG_DEBUG(LogTest, "Hash of second pointer = {}", secondHash);

    ASSERT(hashPointer(&first) == firstHash);
    ASSERT(hashPointer(&second) == secondHash);
}


void Test_Hash_CustomType() {
    LOG_DEBUG(LogTest, "Testing Hash<T> for an arbitrary user-defined type...");

    TestHashKey first {10, 20};
    TestHashKey same {10, 20};
    TestHashKey different {10, 21};

    Hash<TestHashKey> hash;

    u64 firstHash = hash(first);
    u64 sameHash = hash(same);
    u64 differentHash = hash(different);

    LOG_DEBUG(
        LogTest,
        "Hash<TestHashKey>{{10, 20}} = {}",
        firstHash
    );

    LOG_DEBUG(
        LogTest,
        "Hash<TestHashKey>{{10, 20}} again = {}",
        sameHash
    );

    LOG_DEBUG(
        LogTest,
        "Hash<TestHashKey>{{10, 21}} = {}",
        differentHash
    );

    // Equal objects MUST have equal hashes.
    ASSERT(first == same);
    ASSERT(firstHash == sameHash);

    // Unequal objects are NOT required to have different hashes.
    //
    // Therefore, don't write:
    //
    // ASSERT(firstHash != differentHash);
    //
    // It may happen to be true, but collision-freedom is not a correctness
    // requirement for a hash function.
    LOG_DEBUG(
        LogTest,
        "Custom hash equality contract verified"
    );
}


// ============================================================================
// Set - primitive values
// ============================================================================

void Test_Set_Empty() {
    LOG_DEBUG(LogTest, "Testing Set empty state...");

    Set<int> set;

    ASSERT(set.IsEmpty());
    ASSERT(set.Size() == 0);
    ASSERT(!set.Contains(0));
    ASSERT(!set.Contains(123));

    LOG_DEBUG(
        LogTest,
        "Empty Set: size={}, empty={}",
        set.Size(),
        set.IsEmpty()
    );
}


void Test_Set_AddAndContains() {
    LOG_DEBUG(LogTest, "Testing Set Add/Contains...");

    Set<int> set;

    set.Add(10);
    set.Add(20);
    set.Add(30);

    ASSERT(!set.IsEmpty());
    ASSERT(set.Size() == 3);

    ASSERT(set.Contains(10));
    ASSERT(set.Contains(20));
    ASSERT(set.Contains(30));

    ASSERT(!set.Contains(0));
    ASSERT(!set.Contains(-1));
    ASSERT(!set.Contains(40));

    LOG_DEBUG(LogTest, "Set contains {} elements", set.Size());
}


void Test_Set_DuplicateInsertion() {
    LOG_DEBUG(LogTest, "Testing Set duplicate insertion...");

    Set<int> set;

    set.Add(42);
    ASSERT(set.Size() == 1);
    ASSERT(set.Contains(42));

    set.Add(42);
    ASSERT(set.Size() == 1);

    set.Add(42);
    ASSERT(set.Size() == 1);

    LOG_DEBUG(
        LogTest,
        "Repeatedly adding the same value leaves Set size at {}",
        set.Size()
    );
}


void Test_Set_Remove() {
    LOG_DEBUG(LogTest, "Testing Set Remove...");

    Set<int> set;

    set.Add(10);
    set.Add(20);
    set.Add(30);

    ASSERT(set.Remove(20));
    ASSERT(set.Size() == 2);
    ASSERT(!set.Contains(20));

    ASSERT(set.Contains(10));
    ASSERT(set.Contains(30));

    // Removing an element that isn't present should fail.
    ASSERT(!set.Remove(20));
    ASSERT(!set.Remove(999));

    ASSERT(set.Size() == 2);

    LOG_DEBUG(
        LogTest,
        "Set removal verified; remaining size={}",
        set.Size()
    );
}


void Test_Set_RemoveFirstMiddleLast() {
    LOG_DEBUG(LogTest, "Testing Set removal from different bucket positions...");

    Set<int> set;

    for (int i = 0; i < 32; ++i)
        set.Add(i);

    ASSERT(set.Size() == 32);

    ASSERT(set.Remove(0));
    ASSERT(set.Remove(15));
    ASSERT(set.Remove(31));

    ASSERT(set.Size() == 29);

    ASSERT(!set.Contains(0));
    ASSERT(!set.Contains(15));
    ASSERT(!set.Contains(31));

    for (int i = 1; i < 31; ++i) {
        if (i != 15)
            ASSERT(set.Contains(i));
    }

    LOG_DEBUG(
        LogTest,
        "Removal from a populated Set verified; size={}",
        set.Size()
    );
}


void Test_Set_Clear() {
    LOG_DEBUG(LogTest, "Testing Set Clear...");

    Set<int> set;

    for (int i = 0; i < 100; ++i)
        set.Add(i);

    ASSERT(set.Size() == 100);
    ASSERT(!set.IsEmpty());

    set.Clear();

    ASSERT(set.Size() == 0);
    ASSERT(set.IsEmpty());

    for (int i = 0; i < 100; ++i)
        ASSERT(!set.Contains(i));

    LOG_DEBUG(
        LogTest,
        "Set cleared successfully; size={}",
        set.Size()
    );
}


void Test_Set_Rehash() {
    LOG_DEBUG(LogTest, "Testing Set rehashing...");

    Set<int> set;

    // Initial bucket count is 8 and rehashing occurs at a 75% load factor.
    // Insert enough elements to force multiple rehashes.
    constexpr i32 count = 1000;

    for (i32 i = 0; i < count; ++i)
        set.Add(i);

    ASSERT(set.Size() == count);

    for (i32 i = 0; i < count; ++i)
        ASSERT(set.Contains(i));

    LOG_DEBUG(
        LogTest,
        "Set rehash test passed with {} elements",
        set.Size()
    );
}


void Test_Set_Reserve() {
    LOG_DEBUG(LogTest, "Testing Set Reserve...");

    Set<int> set;

    set.Reserve(1000);

    for (i32 i = 0; i < 1000; ++i)
        set.Add(i);

    ASSERT(set.Size() == 1000);

    for (i32 i = 0; i < 1000; ++i)
        ASSERT(set.Contains(i));

    LOG_DEBUG(
        LogTest,
        "Set Reserve verified with {} elements",
        set.Size()
    );
}


void Test_Set_Iteration() {
    LOG_DEBUG(LogTest, "Testing Set iteration...");

    Set<int> set;

    constexpr i32 count = 100;

    for (i32 i = 0; i < count; ++i)
        set.Add(i);

    i32 visited = 0;

    for (int value : set) {
        LOG_DEBUG(LogTest, "Set iteration value={}", value);

        ASSERT(value >= 0);
        ASSERT(value < count);

        ++visited;
    }

    ASSERT(visited == count);

    LOG_DEBUG(
        LogTest,
        "Set iteration visited {} elements",
        visited
    );
}


// ============================================================================
// Set - collision behavior
// ============================================================================

void Test_Set_Collisions() {
    LOG_DEBUG(LogTest, "Testing Set with deliberate hash collisions...");

    Set<int, ConstantHash> set;

    // Every value hashes to exactly the same value/bucket.
    for (i32 i = 0; i < 100; ++i)
        set.Add(i);

    ASSERT(set.Size() == 100);

    for (i32 i = 0; i < 100; ++i)
        ASSERT(set.Contains(i));

    LOG_DEBUG(
        LogTest,
        "Set collision test passed with {} values in one hash bucket",
        set.Size()
    );

    ASSERT(set.Remove(0));
    ASSERT(set.Remove(50));
    ASSERT(set.Remove(99));

    ASSERT(!set.Contains(0));
    ASSERT(!set.Contains(50));
    ASSERT(!set.Contains(99));

    ASSERT(set.Size() == 97);

    for (i32 i = 1; i < 99; ++i) {
        if (i != 50)
            ASSERT(set.Contains(i));
    }

    LOG_DEBUG(
        LogTest,
        "Set collision removal test passed; size={}",
        set.Size()
    );
}


// ============================================================================
// Set - arbitrary types
// ============================================================================

void Test_Set_CustomType() {
    LOG_DEBUG(LogTest, "Testing Set with arbitrary user-defined type...");

    Set<TestHashKey> set;

    TestHashKey a {1, 100};
    TestHashKey b {2, 100};
    TestHashKey c {3, 200};

    set.Add(a);
    set.Add(b);
    set.Add(c);

    ASSERT(set.Size() == 3);

    ASSERT(set.Contains(a));
    ASSERT(set.Contains(b));
    ASSERT(set.Contains(c));

    ASSERT(!set.Contains(TestHashKey{999, 999}));

    // Equal key should not be inserted twice.
    set.Add(TestHashKey{1, 100});

    ASSERT(set.Size() == 3);

    LOG_DEBUG(
        LogTest,
        "Custom-type Set contains {} unique values",
        set.Size()
    );
}


void Test_Set_CustomTypeCollision() {
    LOG_DEBUG(
        LogTest,
        "Testing arbitrary Set type with deliberate collisions..."
    );

    Set<TestHashKey, ConstantHash> set;

    for (i32 i = 0; i < 100; ++i)
        set.Add(TestHashKey{i, i * 10});

    ASSERT(set.Size() == 100);

    for (i32 i = 0; i < 100; ++i)
        ASSERT(set.Contains(TestHashKey{i, i * 10}));

    LOG_DEBUG(
        LogTest,
        "Custom-type collision test passed with {} elements",
        set.Size()
    );
}


// ============================================================================
// Map - primitive keys
// ============================================================================

void Test_Map_Empty() {
    LOG_DEBUG(LogTest, "Testing Map empty state...");

    Map<int, int> map;

    ASSERT(map.IsEmpty());
    ASSERT(map.Size() == 0);

    ASSERT(!map.Contains(0));
    ASSERT(map.Get(0) == nullptr);

    LOG_DEBUG(
        LogTest,
        "Empty Map: size={}, empty={}",
        map.Size(),
        map.IsEmpty()
    );
}


void Test_Map_InsertAndGet() {
    LOG_DEBUG(LogTest, "Testing Map Insert/Get...");

    Map<int, int> map;

    map.Insert(10, 100);
    map.Insert(20, 200);
    map.Insert(30, 300);

    ASSERT(map.Size() == 3);

    ASSERT(map.Contains(10));
    ASSERT(map.Contains(20));
    ASSERT(map.Contains(30));

    ASSERT(!map.Contains(40));

    int* value10 = map.Get(10);
    int* value20 = map.Get(20);
    int* value30 = map.Get(30);

    ASSERT(value10 != nullptr);
    ASSERT(value20 != nullptr);
    ASSERT(value30 != nullptr);

    ASSERT(*value10 == 100);
    ASSERT(*value20 == 200);
    ASSERT(*value30 == 300);

    LOG_DEBUG(
        LogTest,
        "Map values: 10->{}, 20->{}, 30->{}",
        *value10,
        *value20,
        *value30
    );
}


void Test_Map_MissingKey() {
    LOG_DEBUG(LogTest, "Testing Map lookup of missing keys...");

    Map<int, int> map;

    map.Insert(1, 100);
    map.Insert(2, 200);

    ASSERT(map.Get(999) == nullptr);
    ASSERT(!map.Contains(999));

    LOG_DEBUG(
        LogTest,
        "Missing Map key correctly returned nullptr"
    );
}


void Test_Map_UpdateExistingKey() {
    LOG_DEBUG(LogTest, "Testing Map replacement of existing values...");

    Map<int, int> map;

    map.Insert(42, 100);

    ASSERT(map.Size() == 1);
    ASSERT(*map.Get(42) == 100);

    map.Insert(42, 200);

    // Inserting the same key must update rather than add another element.
    ASSERT(map.Size() == 1);
    ASSERT(*map.Get(42) == 200);

    map.Insert(42, 300);

    ASSERT(map.Size() == 1);
    ASSERT(*map.Get(42) == 300);

    LOG_DEBUG(
        LogTest,
        "Map update verified; key 42 now contains {}",
        *map.Get(42)
    );
}


void Test_Map_Remove() {
    LOG_DEBUG(LogTest, "Testing Map Remove...");

    Map<int, int> map;

    map.Insert(10, 100);
    map.Insert(20, 200);
    map.Insert(30, 300);

    ASSERT(map.Remove(20));
    ASSERT(map.Size() == 2);

    ASSERT(!map.Contains(20));
    ASSERT(map.Get(20) == nullptr);

    ASSERT(map.Contains(10));
    ASSERT(map.Contains(30));

    ASSERT(!map.Remove(20));
    ASSERT(!map.Remove(999));

    ASSERT(map.Size() == 2);

    LOG_DEBUG(
        LogTest,
        "Map removal verified; remaining size={}",
        map.Size()
    );
}


void Test_Map_Clear() {
    LOG_DEBUG(LogTest, "Testing Map Clear...");

    Map<int, int> map;

    for (i32 i = 0; i < 100; ++i)
        map.Insert(i, i * 10);

    ASSERT(map.Size() == 100);

    map.Clear();

    ASSERT(map.Size() == 0);
    ASSERT(map.IsEmpty());

    for (i32 i = 0; i < 100; ++i) {
        ASSERT(!map.Contains(i));
        ASSERT(map.Get(i) == nullptr);
    }

    LOG_DEBUG(
        LogTest,
        "Map cleared successfully"
    );
}


void Test_Map_Rehash() {
    LOG_DEBUG(LogTest, "Testing Map rehashing...");

    Map<int, int> map;

    constexpr i32 count = 1000;

    for (i32 i = 0; i < count; ++i)
        map.Insert(i, i * 10);

    ASSERT(map.Size() == count);

    for (i32 i = 0; i < count; ++i) {
        int* value = map.Get(i);

        ASSERT(value != nullptr);
        ASSERT(*value == i * 10);
    }

    LOG_DEBUG(
        LogTest,
        "Map rehash test passed with {} elements",
        map.Size()
    );
}


void Test_Map_Reserve() {
    LOG_DEBUG(LogTest, "Testing Map Reserve...");

    Map<int, int> map;

    map.Reserve(1000);

    for (i32 i = 0; i < 1000; ++i)
        map.Insert(i, i * 2);

    ASSERT(map.Size() == 1000);

    for (i32 i = 0; i < 1000; ++i) {
        int* value = map.Get(i);

        ASSERT(value != nullptr);
        ASSERT(*value == i * 2);
    }

    LOG_DEBUG(
        LogTest,
        "Map Reserve verified with {} elements",
        map.Size()
    );
}


void Test_Map_Iteration() {
    LOG_DEBUG(LogTest, "Testing Map iteration...");

    Map<int, int> map;

    constexpr i32 count = 100;

    for (i32 i = 0; i < count; ++i)
        map.Insert(i, i * 10);

    i32 visited = 0;

    for (auto& entry : map) {
        LOG_DEBUG(
            LogTest,
            "Map iteration key={}, value={}",
            entry.key,
            entry.value
        );

        ASSERT(entry.value == entry.key * 10);

        ++visited;
    }

    ASSERT(visited == count);

    LOG_DEBUG(
        LogTest,
        "Map iteration visited {} entries",
        visited
    );
}


// ============================================================================
// Map - collision behavior
// ============================================================================

void Test_Map_Collisions() {
    LOG_DEBUG(LogTest, "Testing Map with deliberate hash collisions...");

    Map<int, int, ConstantHash> map;

    for (i32 i = 0; i < 100; ++i)
        map.Insert(i, i * 100);

    ASSERT(map.Size() == 100);

    for (i32 i = 0; i < 100; ++i) {
        int* value = map.Get(i);

        ASSERT(value != nullptr);
        ASSERT(*value == i * 100);
    }

    LOG_DEBUG(
        LogTest,
        "Map collision lookup test passed with {} entries",
        map.Size()
    );

    // Update a key while everything remains in one bucket.
    map.Insert(50, 999999);

    ASSERT(map.Size() == 100);
    ASSERT(*map.Get(50) == 999999);

    // Remove entries from the collision chain.
    ASSERT(map.Remove(0));
    ASSERT(map.Remove(50));
    ASSERT(map.Remove(99));

    ASSERT(map.Size() == 97);

    ASSERT(map.Get(0) == nullptr);
    ASSERT(map.Get(50) == nullptr);
    ASSERT(map.Get(99) == nullptr);

    ASSERT(*map.Get(1) == 100);
    ASSERT(*map.Get(98) == 9800);

    LOG_DEBUG(
        LogTest,
        "Map collision update/removal test passed; size={}",
        map.Size()
    );
}


// ============================================================================
// Map - arbitrary types
// ============================================================================

void Test_Map_CustomKey() {
    LOG_DEBUG(LogTest, "Testing Map with arbitrary user-defined keys...");

    Map<TestHashKey, i32> map;

    TestHashKey alice {1, 10};
    TestHashKey bob {2, 10};
    TestHashKey charlie {3, 20};

    map.Insert(alice, 100);
    map.Insert(bob, 200);
    map.Insert(charlie, 300);

    ASSERT(map.Size() == 3);

    ASSERT(map.Contains(alice));
    ASSERT(map.Contains(bob));
    ASSERT(map.Contains(charlie));

    ASSERT(*map.Get(alice) == 100);
    ASSERT(*map.Get(bob) == 200);
    ASSERT(*map.Get(charlie) == 300);

    // Equal custom keys must update.
    map.Insert(TestHashKey{2, 10}, 999);

    ASSERT(map.Size() == 3);
    ASSERT(*map.Get(bob) == 999);

    LOG_DEBUG(
        LogTest,
        "Custom-key Map test passed; size={}",
        map.Size()
    );
}


void Test_Map_CustomKeyCollision() {
    LOG_DEBUG(
        LogTest,
        "Testing custom-key Map with deliberate collisions..."
    );

    Map<TestHashKey, i32, ConstantHash> map;

    for (i32 i = 0; i < 100; ++i) {
        TestHashKey key {
            i,
            i * 10
        };

        map.Insert(key, i * 100);
    }

    ASSERT(map.Size() == 100);

    for (i32 i = 0; i < 100; ++i) {
        TestHashKey key {
            i,
            i * 10
        };

        i32* value = map.Get(key);

        ASSERT(value != nullptr);
        ASSERT(*value == i * 100);
    }

    LOG_DEBUG(
        LogTest,
        "Custom-key collision Map test passed"
    );
}


// ============================================================================
// Array tests
// ============================================================================

void Test_Array_Empty() {
    LOG_DEBUG(LogTest, "Testing Array empty state...");

    Array<int> array;

    ASSERT(array.IsEmpty());
    ASSERT(array.Size() == 0);
    ASSERT(array.Data() == nullptr);

    LOG_DEBUG(
        LogTest,
        "Empty Array: size={}, capacity={}",
        array.Size(),
        array.Capacity()
    );
}


void Test_Array_AddAndIndex() {
    LOG_DEBUG(LogTest, "Testing Array Add/indexing...");

    Array<int> array;

    array.Add(10);
    array.Add(20);
    array.Add(30);

    ASSERT(array.Size() == 3);

    ASSERT(array[0] == 10);
    ASSERT(array[1] == 20);
    ASSERT(array[2] == 30);

    LOG_DEBUG(
        LogTest,
        "Array values: {}, {}, {}",
        array[0],
        array[1],
        array[2]
    );
}


void Test_Array_Growth() {
    LOG_DEBUG(LogTest, "Testing Array automatic growth...");

    Array<int> array;

    constexpr i32 count = 1000;

    for (i32 i = 0; i < count; ++i)
        array.Add(i);

    ASSERT(array.Size() == count);
    ASSERT(array.Capacity() >= count);

    for (i32 i = 0; i < count; ++i)
        ASSERT(array[i] == i);

    LOG_DEBUG(
        LogTest,
        "Array growth verified: size={}, capacity={}",
        array.Size(),
        array.Capacity()
    );
}


void Test_Array_InsertAt() {
    LOG_DEBUG(LogTest, "Testing Array InsertAt...");

    Array<int> array {10, 20, 30, 40};

    array.InsertAt(5, 0);

    ASSERT(array.Size() == 5);
    ASSERT(array[0] == 5);
    ASSERT(array[1] == 10);
    ASSERT(array[2] == 20);
    ASSERT(array[3] == 30);
    ASSERT(array[4] == 40);

    array.InsertAt(25, 3);

    ASSERT(array.Size() == 6);
    ASSERT(array[0] == 5);
    ASSERT(array[1] == 10);
    ASSERT(array[2] == 20);
    ASSERT(array[3] == 25);
    ASSERT(array[4] == 30);
    ASSERT(array[5] == 40);

    array.InsertAt(50, array.Size());

    ASSERT(array.Size() == 7);
    ASSERT(array[6] == 50);

    LOG_DEBUG(
        LogTest,
        "Array InsertAt verified; final size={}",
        array.Size()
    );
}


void Test_Array_RemoveAt() {
    LOG_DEBUG(LogTest, "Testing Array RemoveAt...");

    Array<int> array {10, 20, 30, 40, 50};

    array.RemoveAt(0);

    ASSERT(array.Size() == 4);
    ASSERT(array[0] == 20);

    array.RemoveAt(2);

    ASSERT(array.Size() == 3);
    ASSERT(array[0] == 20);
    ASSERT(array[1] == 30);
    ASSERT(array[2] == 50);

    array.RemoveAt(array.Size() - 1);

    ASSERT(array.Size() == 2);
    ASSERT(array[0] == 20);
    ASSERT(array[1] == 30);

    LOG_DEBUG(
        LogTest,
        "Array RemoveAt verified; final size={}",
        array.Size()
    );
}


void Test_Array_PushPop() {
    LOG_DEBUG(LogTest, "Testing Array Push/Pop...");

    Array<int> array;

    array.Push(10);
    array.Push(20);
    array.Push(30);

    ASSERT(array.Size() == 3);

    int value = array.Pop();
    ASSERT(value == 30);
    ASSERT(array.Size() == 2);

    value = array.Pop();
    ASSERT(value == 20);
    ASSERT(array.Size() == 1);

    value = array.Pop();
    ASSERT(value == 10);
    ASSERT(array.Size() == 0);

    ASSERT(array.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Array Push/Pop verified"
    );
}


void Test_Array_ReserveAndResize() {
    LOG_DEBUG(LogTest, "Testing Array Reserve/Resize...");

    Array<int> array;

    array.Reserve(100);

    ASSERT(array.Capacity() >= 100);
    ASSERT(array.Size() == 0);

    array.Resize(50);

    ASSERT(array.Size() == 50);
    ASSERT(array.Capacity() >= 100);

    array.Resize(10);

    ASSERT(array.Size() == 10);
    ASSERT(array.Capacity() >= 100);

    array.Clear();

    ASSERT(array.Size() == 0);
    ASSERT(array.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Array Reserve/Resize verified; capacity={}",
        array.Capacity()
    );
}


void Test_Array_Iteration() {
    LOG_DEBUG(LogTest, "Testing Array iteration...");

    Array<int> array;

    for (i32 i = 0; i < 100; ++i)
        array.Add(i);

    i32 index = 0;

    for (int value : array) {
        ASSERT(value == index);
        ++index;
    }

    ASSERT(index == 100);

    LOG_DEBUG(
        LogTest,
        "Array iteration visited {} elements",
        index
    );
}


// ============================================================================
// FixedArray tests
// ============================================================================

void Test_FixedArray_Empty() {
    LOG_DEBUG(LogTest, "Testing FixedArray empty state...");

    FixedArray<int, 8> array;

    ASSERT(array.IsEmpty());
    ASSERT(!array.IsFull());
    ASSERT(array.Size() == 0);
    ASSERT(array.Capacity() == 8);

    LOG_DEBUG(
        LogTest,
        "FixedArray: size={}, capacity={}",
        array.Size(),
        array.Capacity()
    );
}


void Test_FixedArray_Add() {
    LOG_DEBUG(LogTest, "Testing FixedArray Add...");

    FixedArray<int, 4> array;

    array.Add(10);
    array.Add(20);
    array.Add(30);
    array.Add(40);

    ASSERT(array.Size() == 4);
    ASSERT(array.IsFull());

    ASSERT(array[0] == 10);
    ASSERT(array[1] == 20);
    ASSERT(array[2] == 30);
    ASSERT(array[3] == 40);

    LOG_DEBUG(
        LogTest,
        "FixedArray reached full capacity of {}",
        array.Capacity()
    );
}


void Test_FixedArray_InitializerList() {
    LOG_DEBUG(LogTest, "Testing FixedArray initializer list...");

    FixedArray<int, 5> array {10, 20, 30};

    ASSERT(array.Size() == 3);
    ASSERT(array.Capacity() == 5);

    ASSERT(array[0] == 10);
    ASSERT(array[1] == 20);
    ASSERT(array[2] == 30);

    LOG_DEBUG(
        LogTest,
        "FixedArray initializer list size={}",
        array.Size()
    );
}


void Test_FixedArray_PushPop() {
    LOG_DEBUG(LogTest, "Testing FixedArray Push/Pop...");

    FixedArray<int, 4> array;

    array.Push(10);
    array.Push(20);
    array.Push(30);

    ASSERT(array.Size() == 3);

    ASSERT(array.Pop() == 30);
    ASSERT(array.Pop() == 20);
    ASSERT(array.Pop() == 10);

    ASSERT(array.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "FixedArray Push/Pop verified"
    );
}


void Test_FixedArray_RemoveAt() {
    LOG_DEBUG(LogTest, "Testing FixedArray RemoveAt...");

    FixedArray<int, 8> array {
        10, 20, 30, 40, 50
    };

    array.RemoveAt(2);

    ASSERT(array.Size() == 4);

    ASSERT(array[0] == 10);
    ASSERT(array[1] == 20);
    ASSERT(array[2] == 40);
    ASSERT(array[3] == 50);

    LOG_DEBUG(
        LogTest,
        "FixedArray RemoveAt verified"
    );
}


void Test_FixedArray_ClearAndReuse() {
    LOG_DEBUG(LogTest, "Testing FixedArray Clear/reuse...");

    FixedArray<int, 4> array {
        1, 2, 3, 4
    };

    ASSERT(array.IsFull());

    array.Clear();

    ASSERT(array.IsEmpty());
    ASSERT(!array.IsFull());
    ASSERT(array.Size() == 0);
    ASSERT(array.Capacity() == 4);

    array.Add(100);
    array.Add(200);

    ASSERT(array.Size() == 2);
    ASSERT(array[0] == 100);
    ASSERT(array[1] == 200);

    LOG_DEBUG(
        LogTest,
        "FixedArray Clear/reuse verified; size={}",
        array.Size()
    );
}


void Test_FixedArray_Iteration() {
    LOG_DEBUG(LogTest, "Testing FixedArray iteration...");

    FixedArray<int, 8> array {
        10, 20, 30, 40, 50
    };

    i32 index = 0;
    i32 expected = 10;

    for (int value : array) {
        ASSERT(value == expected);

        ++index;
        expected += 10;
    }

    ASSERT(index == array.Size());

    LOG_DEBUG(
        LogTest,
        "FixedArray iteration visited {} elements",
        index
    );
}


// ============================================================================
// Queue tests
// ============================================================================

void Test_Queue_Empty() {
    LOG_DEBUG(LogTest, "Testing Queue empty state...");

    Queue<int> queue;

    ASSERT(queue.IsEmpty());
    ASSERT(queue.Size() == 0);

    LOG_DEBUG(
        LogTest,
        "Empty Queue: size={}, capacity={}",
        queue.Size(),
        queue.Capacity()
    );
}


void Test_Queue_EnqueueFrontBack() {
    LOG_DEBUG(LogTest, "Testing Queue Enqueue/Front/Back...");

    Queue<int> queue;

    queue.Enqueue(10);

    ASSERT(queue.Size() == 1);
    ASSERT(queue.Front() == 10);
    ASSERT(queue.Back() == 10);

    queue.Enqueue(20);
    queue.Enqueue(30);

    ASSERT(queue.Size() == 3);
    ASSERT(queue.Front() == 10);
    ASSERT(queue.Back() == 30);

    LOG_DEBUG(
        LogTest,
        "Queue front={}, back={}, size={}",
        queue.Front(),
        queue.Back(),
        queue.Size()
    );
}


void Test_Queue_FIFO() {
    LOG_DEBUG(LogTest, "Testing Queue FIFO ordering...");

    Queue<int> queue;

    for (i32 i = 0; i < 100; ++i)
        queue.Enqueue(i);

    for (i32 expected = 0; expected < 100; ++expected) {
        ASSERT(queue.Front() == expected);

        int output {};
        queue.Dequeue(output);

        ASSERT(output == expected);
    }

    ASSERT(queue.IsEmpty());
    ASSERT(queue.Size() == 0);

    LOG_DEBUG(
        LogTest,
        "Queue FIFO ordering verified for {} elements",
        100
    );
}


void Test_Queue_WrapAround() {
    LOG_DEBUG(LogTest, "Testing Queue circular wrap-around...");

    Queue<int> queue;

    // Fill the queue.
    for (i32 i = 0; i < 8; ++i)
        queue.Enqueue(i);

    // Remove several elements so head moves forward.
    for (i32 expected = 0; expected < 5; ++expected) {
        int output {};
        queue.Dequeue(output);
        ASSERT(output == expected);
    }

    ASSERT(queue.Front() == 5);
    ASSERT(queue.Back() == 7);

    // These writes should occur after the physical end of the array and
    // exercise the circular-buffer wrapping behavior.
    queue.Enqueue(8);
    queue.Enqueue(9);
    queue.Enqueue(10);
    queue.Enqueue(11);
    queue.Enqueue(12);

    ASSERT(queue.Size() == 8);
    ASSERT(queue.Front() == 5);
    ASSERT(queue.Back() == 12);

    for (i32 expected = 5; expected <= 12; ++expected) {
        int output {};
        queue.Dequeue(output);

        ASSERT(output == expected);
    }

    ASSERT(queue.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Queue wrap-around behavior verified"
    );
}


void Test_Queue_GrowthAfterWrapAround() {
    LOG_DEBUG(
        LogTest,
        "Testing Queue growth after circular wrap-around..."
    );

    Queue<int> queue;

    // Establish a non-zero head.
    for (i32 i = 0; i < 3; ++i)
        queue.Enqueue(i);

    for (i32 i = 0; i < 2; ++i) {
        int output {};
        queue.Dequeue(output);

        ASSERT(output == i);
    }

    // The logical queue currently contains [2].
    // Add enough values to trigger growth while head is not zero.
    for (i32 i = 3; i < 100; ++i)
        queue.Enqueue(i);

    ASSERT(queue.Size() == 98);

    for (i32 expected = 2; expected < 100; ++expected) {
        int output {};
        queue.Dequeue(output);

        ASSERT(output == expected);
    }

    ASSERT(queue.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Queue growth with non-zero head verified"
    );
}


void Test_Queue_ClearAndReuse() {
    LOG_DEBUG(LogTest, "Testing Queue Clear/reuse...");

    Queue<int> queue;

    for (i32 i = 0; i < 100; ++i)
        queue.Enqueue(i);

    ASSERT(queue.Size() == 100);

    queue.Clear();

    ASSERT(queue.IsEmpty());
    ASSERT(queue.Size() == 0);

    queue.Enqueue(100);
    queue.Enqueue(200);

    ASSERT(queue.Size() == 2);
    ASSERT(queue.Front() == 100);
    ASSERT(queue.Back() == 200);

    int output {};
    queue.Dequeue(output);
    ASSERT(output == 100);

    queue.Dequeue(output);
    ASSERT(output == 200);

    ASSERT(queue.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Queue Clear/reuse verified"
    );
}


void Test_Queue_Reserve() {
    LOG_DEBUG(LogTest, "Testing Queue Reserve...");

    Queue<int> queue;

    queue.Reserve(1000);

    ASSERT(queue.Capacity() >= 1000);
    ASSERT(queue.Size() == 0);

    for (i32 i = 0; i < 1000; ++i)
        queue.Enqueue(i);

    ASSERT(queue.Size() == 1000);

    for (i32 expected = 0; expected < 1000; ++expected) {
        int output {};
        queue.Dequeue(output);

        ASSERT(output == expected);
    }

    ASSERT(queue.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Queue Reserve verified with capacity={}",
        queue.Capacity()
    );
}


// ============================================================================
// Copy / move tests
// ============================================================================

void Test_Array_CopyAndMove() {
    LOG_DEBUG(LogTest, "Testing Array copy/move semantics...");

    Array<int> original {
        10, 20, 30
    };

    Array<int> copy = original;

    ASSERT(copy.Size() == 3);
    ASSERT(copy[0] == 10);
    ASSERT(copy[1] == 20);
    ASSERT(copy[2] == 30);

    copy[0] = 999;

    // Verify that the copy owns independent storage.
    ASSERT(original[0] == 10);
    ASSERT(copy[0] == 999);

    Array<int> moved = Move(original);

    ASSERT(moved.Size() == 3);
    ASSERT(moved[0] == 10);
    ASSERT(moved[1] == 20);
    ASSERT(moved[2] == 30);

    ASSERT(original.Size() == 0);
    ASSERT(original.Data() == nullptr);

    LOG_DEBUG(
        LogTest,
        "Array copy/move semantics verified"
    );
}


void Test_Queue_CopyAndMove() {
    LOG_DEBUG(LogTest, "Testing Queue copy/move semantics...");

    Queue<int> original;

    for (i32 i = 0; i < 20; ++i)
        original.Enqueue(i);

    // Move the head forward so copy has to account for physical indexing.
    for (i32 i = 0; i < 7; ++i) {
        int output {};
        original.Dequeue(output);
        ASSERT(output == i);
    }

    Queue<int> copy = original;

    ASSERT(copy.Size() == original.Size());

    for (i32 expected = 7; expected < 20; ++expected) {
        int output {};
        copy.Dequeue(output);

        ASSERT(output == expected);
    }

    Queue<int> moved = Move(original);

    ASSERT(moved.Size() == 13);
    ASSERT(original.Size() == 0);
    ASSERT(original.IsEmpty());

    for (i32 expected = 7; expected < 20; ++expected) {
        int output {};
        moved.Dequeue(output);

        ASSERT(output == expected);
    }

    ASSERT(moved.IsEmpty());

    LOG_DEBUG(
        LogTest,
        "Queue copy/move semantics verified"
    );
}


// ============================================================================
// Lifetime tests
// ============================================================================

void Test_Array_ObjectLifetime() {
    LOG_DEBUG(LogTest, "Testing Array object lifetime...");

    TestObject::ResetCounters();

    {
        Array<TestObject> array;

        array.Emplace(10);
        array.Emplace(20);
        array.Emplace(30);

        ASSERT(array.Size() == 3);

        LOG_DEBUG(
            LogTest,
            "Array<TestObject>: constructed={}, destroyed={}",
            TestObject::constructed,
            TestObject::destroyed
        );

        array.RemoveAt(1);

        ASSERT(array.Size() == 2);
    }

    ASSERT(TestObject::constructed == TestObject::destroyed);

    LOG_DEBUG(
        LogTest,
        "Array<TestObject> lifetime balanced: constructed={}, destroyed={}",
        TestObject::constructed,
        TestObject::destroyed
    );
}


void Test_Queue_ObjectLifetime() {
    LOG_DEBUG(LogTest, "Testing Queue object lifetime...");

    TestObject::ResetCounters();

    {
        Queue<TestObject> queue;

        queue.Enqueue(TestObject{10});
        queue.Enqueue(TestObject{20});
        queue.Enqueue(TestObject{30});

        ASSERT(queue.Size() == 3);

        TestObject output;

        queue.Dequeue(output);
        ASSERT(output.value == 10);

        queue.Dequeue(output);
        ASSERT(output.value == 20);

        queue.Clear();

        ASSERT(queue.IsEmpty());
    }

    ASSERT(TestObject::constructed == TestObject::destroyed);

    LOG_DEBUG(
        LogTest,
        "Queue<TestObject> lifetime balanced: constructed={}, destroyed={}",
        TestObject::constructed,
        TestObject::destroyed
    );
}


// ============================================================================
// Combined smoke test
// ============================================================================

void Test_AllContainers_SmokeTest() {
    LOG_DEBUG(LogTest, "Running combined container smoke test...");

    Array<int> array {1, 2, 3};

    FixedArray<int, 3> fixedArray {4, 5, 6};

    Queue<int> queue;
    queue.Enqueue(7);
    queue.Enqueue(8);
    queue.Enqueue(9);

    Set<int> set;
    set.Add(10);
    set.Add(11);
    set.Add(12);

    Map<int, int> map;
    map.Insert(13, 130);
    map.Insert(14, 140);
    map.Insert(15, 150);

    ASSERT(array.Size() == 3);
    ASSERT(fixedArray.Size() == 3);

    ASSERT(queue.Size() == 3);
    ASSERT(queue.Front() == 7);
    ASSERT(queue.Back() == 9);

    ASSERT(set.Size() == 3);
    ASSERT(set.Contains(10));
    ASSERT(set.Contains(11));
    ASSERT(set.Contains(12));

    ASSERT(map.Size() == 3);
    ASSERT(*map.Get(13) == 130);
    ASSERT(*map.Get(14) == 140);
    ASSERT(*map.Get(15) == 150);

    LOG_DEBUG(
        LogTest,
        "Combined smoke test passed: Array={}, FixedArray={}, Queue={}, Set={}, Map={}",
        array.Size(),
        fixedArray.Size(),
        queue.Size(),
        set.Size(),
        map.Size()
    );
}


// ============================================================================
// Suggested master test entry point
// ============================================================================

void RunContainerTests() {
    LOG_DEBUG(LogTest, "==================================================");
    LOG_DEBUG(LogTest, "Beginning container tests");
    LOG_DEBUG(LogTest, "==================================================");

    // Hash
    Test_Hash_MixBits();
    Test_Hash_AllTrivialIntegerTypes();
    Test_Hash_IntegerEdgeCases();
    Test_Hash_Pointers();
    Test_Hash_CustomType();

    // Set
    Test_Set_Empty();
    Test_Set_AddAndContains();
    Test_Set_DuplicateInsertion();
    Test_Set_Remove();
    Test_Set_RemoveFirstMiddleLast();
    Test_Set_Clear();
    Test_Set_Rehash();
    Test_Set_Reserve();
    Test_Set_Iteration();
    Test_Set_Collisions();
    Test_Set_CustomType();
    Test_Set_CustomTypeCollision();

    // Map
    Test_Map_Empty();
    Test_Map_InsertAndGet();
    Test_Map_MissingKey();
    Test_Map_UpdateExistingKey();
    Test_Map_Remove();
    Test_Map_Clear();
    Test_Map_Rehash();
    Test_Map_Reserve();
    Test_Map_Iteration();
    Test_Map_Collisions();
    Test_Map_CustomKey();
    Test_Map_CustomKeyCollision();

    // Array
    Test_Array_Empty();
    Test_Array_AddAndIndex();
    Test_Array_Growth();
    Test_Array_InsertAt();
    Test_Array_RemoveAt();
    Test_Array_PushPop();
    Test_Array_ReserveAndResize();
    Test_Array_Iteration();

    // FixedArray
    Test_FixedArray_Empty();
    Test_FixedArray_Add();
    Test_FixedArray_InitializerList();
    Test_FixedArray_PushPop();
    Test_FixedArray_RemoveAt();
    Test_FixedArray_ClearAndReuse();
    Test_FixedArray_Iteration();

    // Queue
    Test_Queue_Empty();
    Test_Queue_EnqueueFrontBack();
    Test_Queue_FIFO();
    Test_Queue_WrapAround();
    Test_Queue_GrowthAfterWrapAround();
    Test_Queue_ClearAndReuse();
    Test_Queue_Reserve();

    // Copy/move
    Test_Array_CopyAndMove();
    Test_Queue_CopyAndMove();

    // Lifetime
    Test_Array_ObjectLifetime();
    Test_Queue_ObjectLifetime();

    // Everything together
    Test_AllContainers_SmokeTest();

    LOG_DEBUG(LogTest, "==================================================");
    LOG_DEBUG(LogTest, "All container tests completed");
    LOG_DEBUG(LogTest, "==================================================");
}
