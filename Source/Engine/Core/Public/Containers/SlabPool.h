// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "CoreMinimal.h"

template<typename ElementType, u32 BlockSize = 32>
class SlabPool {
    struct Block;
    struct SlotHeader {
        Block* block;
        SlotHeader* nextFree;
    };
    struct Slot {
        SlotHeader header;
        alignas(ElementType) u8 storage[sizeof(ElementType)];
    };
    struct Block {
        StaticArray<Slot, BlockSize> slots;
        u32 liveCount = 0;
        u32 bumpCursor = 0;
    };

public:
    template<typename... Args>
    ElementType* Allocate(Args&&... args) {
        Slot* slot = AcquireSlot();
        return new (slot->storage) ElementType(std::forward<Args>(args)...);
    }

    void Destroy(ElementType* element) {
        element->~ElementType();
        auto* slot = reinterpret_cast<Slot*>(
            reinterpret_cast<u8*>(element) - offsetof(Slot, storage));
        ReleaseSlot(slot);
    }

private:
    Slot* AcquireSlot() {
        if (freeListHead) {
            Slot* slot = reinterpret_cast<Slot*>(freeListHead);
            freeListHead = freeListHead->nextFree;
            ++slot->header.block->liveCount;
            return slot;
        }

        if (blocks.IsEmpty() || blocks.Back()->bumpCursor == BlockSize)
            blocks.Add(MakeUnique<Block>());

        Block* block = blocks.Back().get();
        Slot* slot = &block->slots[block->bumpCursor++];
        slot->header.block = block;
        ++block->liveCount;
        return slot;
    }

    void ReleaseSlot(Slot* slot) {
        Block* block = slot->header.block;
        slot->header.nextFree = freeListHead;
        freeListHead = &slot->header;

        if (--block->liveCount == 0 && blocks.Size() > 1)
            FreeBlock(block);
    }

    void FreeBlock(Block* block) {
        SlotHeader** cursor = &freeListHead;
        while (*cursor) {
            if (reinterpret_cast<Slot*>(*cursor)->header.block == block)
                *cursor = (*cursor)->nextFree;
            else
                cursor = &(*cursor)->nextFree;
        }
        blocks.RemoveIf([&](UniquePtr<Block> const& b) { return b.get() == block; });
    }

private:
    Array<UniquePtr<Block>> blocks;
    SlotHeader* freeListHead {nullptr};
};