#pragma once

#include <cz/allocator.hpp>
#include <cz/str.hpp>
#include "bit_array.hpp"
#include "hash.hpp"
#include "next_power_of_two.hpp"

namespace cz {

struct StrSet {
    cz::Str* keys;
    unsigned char* _masks;
    size_t cap;
    size_t count;

    void drop(cz::Allocator allocator) {
        allocator.dealloc({keys, sizeof(cz::Str) * cap});
        allocator.dealloc({_masks, bit_array::alloc_size(cap)});
    }

    void reserve(cz::Allocator allocator, size_t extra) {
        if (count + extra + cap / 4 >= cap) {
            StrSet new_this;
            new_this.cap = next_power_of_two(count + extra);
            new_this.count = 0;

            new_this.keys = static_cast<cz::Str*>(
                allocator.alloc({sizeof(cz::Str) * new_this.cap, alignof(cz::Str)}).buffer);
            new_this._masks = static_cast<unsigned char*>(
                allocator.alloc({sizeof(unsigned char) * new_this.cap, alignof(unsigned char)})
                    .buffer);

            CZ_ASSERT(new_this.keys);
            CZ_ASSERT(new_this._masks);
            memset(new_this._masks, 0, sizeof(unsigned char) * new_this.cap);

            if (count != 0) {
                for (size_t i = 0; i < cap; ++i) {
                    if (is_present(i)) {
                        new_this.insert_hash(keys[i]);
                    }
                }
            }

            if (cap != 0) {
                allocator.dealloc({keys, sizeof(cz::Str) * cap});
                allocator.dealloc({_masks, sizeof(unsigned char) * cap});
            }

            *this = new_this;
        }
    }

    bool is_present(size_t index) { return bit_array::get(_masks, index); }
    void set_present(size_t index) { return bit_array::set(_masks, index); }
    void set_removed(size_t index) { return bit_array::unset(_masks, index); }

    void clear() { memset(_masks, 0, sizeof(unsigned char) * cap); }

    static Hash hash(cz::Str key) { return ::cz::hash(key, 0x7521AB297521AB29); }

    cz::Str* get_hash(cz::Str key) { return get(key, hash(key)); }

    cz::Str* get(cz::Str key, Hash hash) {
        if (count == 0) {
            return nullptr;
        }

        CZ_DEBUG_ASSERT(cap > 0);
        CZ_DEBUG_ASSERT(next_power_of_two(cap - 1) == cap);

        size_t index = hash & (cap - 1);
        CZ_DEBUG_ASSERT(index == (hash % cap));

        for (size_t offset = 0; offset < count; ++offset) {
            if (is_present(index)) {
                if (keys[index] == key) {
                    return &keys[index];
                } else {
                    ++index;
                    index &= cap - 1;
                    CZ_DEBUG_ASSERT(((hash + offset + 1) % cap) == index);
                }
            } else {
                break;
            }
        }

        return nullptr;
    }

    void insert_hash(cz::Str key) { insert(key, hash(key)); }

    void insert(cz::Str key, Hash hash) {
        CZ_DEBUG_ASSERT(cap - count >= 1);
        CZ_DEBUG_ASSERT(next_power_of_two(cap - 1) == cap);

        size_t index = hash & (cap - 1);
        CZ_DEBUG_ASSERT(index == (hash % cap));

        for (size_t offset = 0; offset <= count; ++offset) {
            if (is_present(index)) {
                ++index;
                index &= cap - 1;
                CZ_DEBUG_ASSERT(((hash + offset + 1) % cap) == index);
            } else {
                keys[index] = key;
                set_present(index);
                ++count;
                break;
            }
        }
    }

    bool remove_hash(cz::Str key) { return remove(key, hash(key)); }

    bool remove(cz::Str key, Hash hash) {
        if (cap == 0) {
            return false;
        }

        size_t index = hash & (cap - 1);

        for (size_t offset = 0; offset <= count; ++offset) {
            if (is_present(index) && key == keys[index]) {
                set_removed(index);
                CZ_DEBUG_ASSERT(count >= 1);
                --count;
                return true;
            } else if (is_present(index)) {
                ++index;
                index &= cap - 1;
            } else {
                return false;
            }
        }

        return false;
    }
};

}
