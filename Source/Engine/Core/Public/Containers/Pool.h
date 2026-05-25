#pragma once

#include "CoreMinimal.h"

template<typename ElementType>
class TPool {
public:
    static constexpr u32 SLOTS_PER_BLOCK = 64;

    ~TPool() {
        for (FBlock* block : blocks) {
            delete block;
        }
    }

    template<typename... TArgs>
    ElementType* Allocate(TArgs&&... args) {
        for (FBlock* block : blocks) {
            if (block->freeCount == 0) continue;
            return block->Allocate(std::forward<TArgs>(args)...);
        }

        FBlock* block = new FBlock();
        blocks.Add(block);
        return block->Allocate(std::forward<TArgs>(args)...);
    }

    void Free(ElementType* ptr) {
        for (FBlock* block : blocks) {
            if (!block->Contains(ptr)) continue;
            block->Free(ptr);
            return;
        }
    }

private:
    struct FBlock {
        alignas(ElementType) u8 data[sizeof(ElementType) * SLOTS_PER_BLOCK] {};
        u32 freeList[SLOTS_PER_BLOCK];
        u32 freeHead {0};
        u32 freeCount {SLOTS_PER_BLOCK};

        FBlock() {
            for (u32 i = 0; i < SLOTS_PER_BLOCK; i++)
                freeList[i] = i + 1;
        }

        template<typename... TArgs>
        ElementType* Allocate(TArgs&&... args) {
            ASSERT(freeCount > 0);
            u32 index = freeHead;
            freeHead = freeList[index];
            freeCount--;
            return new (&data[index * sizeof(ElementType)]) ElementType(std::forward<TArgs>(args)...);
        }

        void Free(ElementType* ptr) {
            u32 index = SlotIndex(ptr);
            freeList[index] = freeHead;
            freeHead = index;
            freeCount++;
            ptr->~ElementType();
        }

        [[nodiscard]] bool Contains(ElementType const* ptr) const {
            ElementType const* begin = reinterpret_cast<ElementType const*>(&data[0]);
            return ptr >= begin && ptr < begin + SLOTS_PER_BLOCK;
        }

        [[nodiscard]] u32 SlotIndex(ElementType const* ptr) const {
            ElementType const* begin = reinterpret_cast<ElementType const*>(&data[0]);
            return static_cast<u32>(ptr - begin);
        }
    };

    TArray<FBlock*> blocks;
};