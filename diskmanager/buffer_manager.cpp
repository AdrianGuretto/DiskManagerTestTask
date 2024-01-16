#include "buffer_manager.hpp"

std::optional<std::reference_wrapper<const DataBlock>> BufferManager::getDataBlock(const size_t block_hash) noexcept{
    // Check if the block exists in cache
    auto found_block_it = blockhash_to_data_.find(block_hash);
    if (found_block_it == blockhash_to_data_.end()){
        return std::nullopt;
    }

    pinBlock(block_hash);

    return found_block_it->second;
}

void BufferManager::removeDataBlock(const size_t block_hash) noexcept{
    blockhash_to_data_.erase(block_hash);
    blockhashes_order_.remove(block_hash);
}

void BufferManager::addDataBlock(const DataBlock& data_block, const size_t data_hash) noexcept{
    // if the block already exists in memory, just pin it in the cache
    if (blockhash_to_data_.count(data_hash)){
        pinBlock(data_hash);
        return;
    }
    blockhash_to_data_[data_hash] = data_block;
    pinBlock(data_hash);
}

const std::list<size_t>& BufferManager::getBlockOrder() const noexcept{
    return blockhashes_order_;
}

const std::unordered_map<size_t, DataBlock>& BufferManager::getCacheDump() const noexcept{
    return blockhash_to_data_;
}

size_t BufferManager::getCacheSize() const noexcept{
    return blockhash_to_data_.size();
}

void BufferManager::pinBlock(const size_t block_hash) noexcept{
    // if the hash does not exist in the cache, we just add it
    if (blockhashes_order_.size() >= MAX_CACHED_BLOCKS_NUMBER){
        deleteLeastRecentlyUsedBlock();
    }

    blockhashes_order_.remove(block_hash);
    blockhashes_order_.push_front(block_hash);
}

void BufferManager::unpinBlock(const size_t block_hash) noexcept{
    blockhashes_order_.remove(block_hash);
    blockhash_to_data_.erase(block_hash);
}

void BufferManager::deleteLeastRecentlyUsedBlock() noexcept{
    if (!blockhashes_order_.empty()){
        size_t lru_hash = blockhashes_order_.back();
        blockhashes_order_.pop_back();
        blockhash_to_data_.erase(lru_hash);
    }
}

void BufferManager::clearBuffer() noexcept{
    blockhashes_order_.clear();
    blockhash_to_data_.clear();
}