#pragma once

#include <cz/allocator.hpp>
#include <cz/str.hpp>
#include "bit_array.hpp"
#include "hash.hpp"
#include "next_power_of_two.hpp"

namespace cz {

template <class Value>
struct Str_Map_Removable {
    cz::Str* keys;
    Value* values;
    unsigned char* _masks;
    size_t cap;
    size_t count;

    void drop(cz::Allocator allocator) {
        allocator.dealloc({keys, sizeof(cz::Str) * cap});
        allocator.dealloc({values, sizeof(Value) * cap});
        allocator.dealloc({_masks, bit_array::alloc_size(2 * cap)});
    }

    void reserve(cz::Allocator allocator, size_t extra) {
        if (count + extra + cap / 4 >= cap) {
            Str_Map_Removable<Value> new_this;
            new_this.cap = next_power_of_two(count + extra);
            new_this.count = 0;

            new_this.keys = static_cast<cz::Str*>(
                allocator.alloc({sizeof(cz::Str) * new_this.cap, alignof(cz::Str)}));
            new_this.values = static_cast<Value*>(
                allocator.alloc({sizeof(Value) * new_this.cap, alignof(Value)}));
            new_this._masks = static_cast<unsigned char*>(
                allocator.alloc({bit_array::alloc_size(2 * new_this.cap), alignof(unsigned char)}));

            CZ_ASSERT(new_this.keys);
            CZ_ASSERT(new_this.values);
            CZ_ASSERT(new_this._masks);
            memset(new_this._masks, 0, bit_array::alloc_size(2 * new_this.cap));

            if (count != 0) {
                for (size_t i = 0; i < cap; ++i) {
                    if (is_present(i)) {
                        new_this.insert_hash(keys[i], values[i]);
                    }
                }
            }

            if (cap != 0) {
                allocator.dealloc({keys, sizeof(cz::Str) * cap});
                allocator.dealloc({values, sizeof(Value) * cap});
                allocator.dealloc({_masks, bit_array::alloc_size(2 * cap)});
            }

            *this = new_this;
        }
    }

    bool is_present(size_t index) const { return bit_array::get(_masks, 2 * index); }
    bool is_tombstone(size_t index) const { return bit_array::get(_masks, 2 * index + 1); }

    void set_present(size_t index) {
        bit_array::set(_masks, 2 * index);
        bit_array::unset(_masks, 2 * index + 1);
    }
    void set_tombstone(size_t index) {
        bit_array::unset(_masks, 2 * index);
        bit_array::set(_masks, 2 * index + 1);
    }

    static Hash hash(cz::Str key) { return ::cz::hash(key, 0x7521AB297521AB29); }

    bool index_of(cz::Str key, Hash hash, size_t* out) const {
        size_t index = hash & (cap - 1);
        CZ_DEBUG_ASSERT(index == (hash % cap));

        for (size_t offset = 0; offset < count; ++offset) {
            if (is_present(index)) {
                if (keys[index] == key) {
                    *out = index;
                    return true;
                } else {
                cont:
                    ++index;
                    index &= cap - 1;
                    CZ_DEBUG_ASSERT(((hash + offset + 1) % cap) == index);
                }
            } else if (is_tombstone(index)) {
                goto cont;
            } else {
                break;
            }
        }
        return false;
    }

    Value* get_hash(cz::Str key) { return get(key, hash(key)); }

    Value* get(cz::Str key, Hash hash) {
        if (count == 0) {
            return nullptr;
        }

        CZ_DEBUG_ASSERT(cap > 0);
        CZ_DEBUG_ASSERT(next_power_of_two(cap - 1) == cap);

        size_t index;
        if (index_of(key, hash, &index)) {
            return &values[index];
        } else {
            return nullptr;
        }
    }

    Value* remove_hash(cz::Str key) { return remove(key, hash(key)); }

    bool remove(cz::Str key, Hash hash) {
        size_t index;
        if (index_of(key, hash, &index)) {
            set_tombstone(index);
            return true;
        } else {
            return false;
        }
    }

    void insert_hash(cz::Str key, const Value& value) { insert(key, hash(key), value); }

    void insert(cz::Str key, Hash hash, const Value& value) {
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
                values[index] = value;
                set_present(index);
                ++count;
                break;
            }
        }
    }
};

}
