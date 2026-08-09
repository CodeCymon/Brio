// Copyright (c) Simon Kirsch 2026.

#include "Containers/Array.h"
#include "Containers/FixedArray.h"
#include "Containers/Map.h"
#include "Containers/Queue.h"
#include "Containers/Set.h"
#include "Containers/Stack.h"
#include "Log/Log.h"



struct Foo {
    i32 number {};
    bool validate {};

    bool operator==(Foo const& other) const {
        return number == other.number && validate == other.validate;
    }
};

template<>
struct Hash<Foo> {
    u64 operator()(Foo const& foo) const {
        return MixBits(foo.number + static_cast<u64>(foo.validate));
    }
};


int main() {
    Array fooArray {
        Foo{.number = 42, .validate = true},
        Foo{.number = 43, .validate = false},
    };
    fooArray.Add({.number = 44, .validate = true});
    for (auto const& elm : fooArray) {
        LOG_DEBUG(LogTest, "Array value: {} - validate {}", elm.number, elm.validate);
    }

    Stack<Foo> stack;
    stack.Push({.number = 767, .validate = true});
    stack.Push({.number = 47456, .validate = false});
    while (!stack.IsEmpty()) {
        Foo x = stack.Pop();
        LOG_DEBUG(LogTest, "Stack value: {} - validate {}", x.number, x.validate);
    }

    Queue<i32> queue;
    queue.Enqueue(13);
    queue.Enqueue(956);
    while (!queue.IsEmpty()) {
        i32 x;
        queue.Dequeue(x);
        LOG_DEBUG(LogTest, "Queue value: {}", x);
    }

    Map<i32, Foo> map;
    map.Insert(3, Foo{.number = 37, .validate = true});
    map.Insert(4, Foo{.number = 68, .validate = false});
    map.Insert(5, Foo{.number = 743, .validate = true});
    map.Remove(4);
    if (map.Contains(3)) {
        LOG_DEBUG(LogTest, "Map contains 3 - value: {}", map.Get(3)->number);
    }

    Foo a {.number = 549, .validate = true};
    Foo b {.number = 763, .validate = false};
    Set<Foo> set;
    set.Add(a);
    set.Add(b);
    set.Remove(a);
    if (set.Contains(Foo{.number = 767, .validate = false})) {
        LOG_DEBUG(LogTest, "Set contains - value: '767', validate 'false'");
    }
    if (set.Contains(Foo{.number = 763, .validate = false})) {
        LOG_DEBUG(LogTest, "Set contains - value: '763', validate 'false'");
    }
    return 0;
}

