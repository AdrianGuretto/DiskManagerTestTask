#pragma once

#include <memory>
#include <iostream>
#include <functional>
#include <string_view>
#include <cstring>
#include <utility>

#define MAX_CACHED_BLOCKS_NUMBER 50          /* limit of cached data blocks */
#define MAX_DATA_BLOCK_SIZE 4096             /* maximum number of bytes a data block can have */

struct DataBlock{
    DataBlock() noexcept{
        // clear the buffer
        std::memset(data, 0x00, MAX_DATA_BLOCK_SIZE);
    }

    DataBlock& operator=(const DataBlock& other){
        this->data_size = other.data_size;
        std::memcpy(this->data, other.data, MAX_DATA_BLOCK_SIZE);
        return *this;
    }

    size_t Hash() const noexcept{
        return std::hash<std::string_view>{}(data);
    }

    size_t data_size;
    char data[MAX_DATA_BLOCK_SIZE];
};


static inline bool operator==(const DataBlock& lhs, const DataBlock& rhs) noexcept{
    return lhs.data_size == rhs.data_size && std::memcmp(lhs.data, rhs.data, MAX_DATA_BLOCK_SIZE) == 0;
}