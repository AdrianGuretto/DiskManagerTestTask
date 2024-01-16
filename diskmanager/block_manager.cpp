#include "block_manager.hpp"

using namespace std::string_literals;

BlockManager::BlockManager(duckdb::DuckDB& db_obj){
    conn_db_ = std::make_unique<duckdb::Connection>(db_obj);
    conn_db_->Query("CREATE TABLE IF NOT EXISTS blocks (block_id INTEGER, data VARCHAR, PRIMARY KEY(block_id));");
}


void BlockManager::writeBlock(const char* data_bytes, const size_t data_size){
    if (!data_bytes || data_size == 0){
        return;
    }

    const std::vector<DataBlock> data_blocks = createDataBlocks(data_bytes, data_size);
    for (const DataBlock& dblock : data_blocks){
        const size_t block_hash = dblock.Hash();
        // Don't write to the file if the block is cached (exists)
        if (buff_manager_.getDataBlock(block_hash).has_value()){
            continue;
        }
        insertDataBlockToDB(dblock, block_hash);
    }
}

bool BlockManager::readBlock(const size_t block_hash, DataBlock& in_block) noexcept{
    std::optional<DataBlock> cached_block = buff_manager_.getDataBlock(block_hash);
    // The block has not been found in the cache
    if (!cached_block.has_value()){
        return false;
    }

    in_block = cached_block.value();
    ++read_blocks_count_;
    return true;
}

void BlockManager::setNewDBObject(duckdb::DuckDB& db_obj) noexcept{
    buff_manager_.clearBuffer();

    conn_db_.reset();
    conn_db_ = std::make_unique<duckdb::Connection>(db_obj);
}

std::vector<DataBlock> BlockManager::createDataBlocks(const char* data, const size_t data_size){
    std::vector<DataBlock> ret_vec;
    if (!data || data_size == 0){
        return ret_vec;
    }

    size_t blocks_num = (data_size + MAX_DATA_BLOCK_SIZE - 1) / MAX_DATA_BLOCK_SIZE; // There has to be at least one data block
    ret_vec.reserve(blocks_num);

    size_t wrote_bytes = 0;
    size_t left_bytes = data_size;

    while (wrote_bytes < data_size){
        DataBlock dblock;
        const size_t copy_size = std::min(left_bytes, static_cast<size_t>(MAX_DATA_BLOCK_SIZE));

        std::memcpy(dblock.data, data + wrote_bytes, copy_size);
        dblock.data_size = copy_size;

        ret_vec.push_back(std::move(dblock));

        wrote_bytes += copy_size;
        left_bytes -= copy_size;
    }

    return ret_vec;
}


void BlockManager::insertDataBlockToDB(const DataBlock& dblock, const size_t block_hash){
    auto res = conn_db_->Query("INSERT INTO blocks VALUES ("s + std::to_string(block_hash) + ", '"s + std::string(dblock.data, MAX_DATA_BLOCK_SIZE) + "', "s + std::to_string(MAX_DATA_BLOCK_SIZE) + ") "s);
    if (res->HasError()){
        throw std::runtime_error("Failed to insert data block to the database file: "s + res->GetError());
    }
    buff_manager_.addDataBlock(dblock, block_hash);
    ++written_blocks_count_;
}

size_t BlockManager::getBufferSize() const noexcept{
    return buff_manager_.getCacheSize();
}

size_t BlockManager::getTotalReadBlocksCount() const noexcept{
    return written_blocks_count_;
}

size_t BlockManager::getTotalWrittenBlocksCount() const noexcept{
    return read_blocks_count_;
}