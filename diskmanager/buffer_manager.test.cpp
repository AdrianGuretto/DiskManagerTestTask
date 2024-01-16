#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "buffer_manager.hpp"

TEST(BufferManagerHappyTests, InitStatTest){
    BufferManager manager;
    EXPECT_EQ(manager.getCacheSize(), static_cast<size_t>(0));
    EXPECT_TRUE(manager.getBlockOrder().empty());
    EXPECT_TRUE(manager.getCacheDump().empty());
    EXPECT_FALSE(manager.getDataBlock(231).has_value());
}

TEST(BufferManagerHappyTests, AddDataBlockTest){
    { // Add a single block
        BufferManager manager;
        DataBlock block;
        block.data_size = 0;
        manager.addDataBlock(block, 0);
        EXPECT_NE(manager.getCacheSize(), static_cast<size_t>(0));
        EXPECT_EQ(manager.getBlockOrder(), std::list<size_t>{0});
        EXPECT_EQ(manager.getCacheDump(), (std::unordered_map<size_t, DataBlock>{{0, block}}));
        EXPECT_TRUE(manager.getDataBlock(0).has_value());
    }

    { // Add multiple blocks
        BufferManager manager;
        const std::string str1 = "This is test string1", str2 = "And this is test string 2";
        DataBlock block1, block2, block3;
        const size_t block1_hash = 1111, block2_hash = 2222, block3_hash = 3333; 
        
        block1.data_size = str1.size();
        std::memcpy(block1.data, str1.data(), str1.size());

        block2.data_size = str2.size();
        std::memcpy(block2.data, str2.data(), str2.size());

        block3.data_size = 3;
        block3.data[0] = 'h';
        block3.data[1] = 'i';
        block3.data[2] = '!';

        const std::unordered_map<size_t, DataBlock> final_cache_dump{
            {block1_hash, block1},
            {block2_hash, block2},
            {block3_hash, block3},
        };

        manager.addDataBlock(block1, block1_hash);
        EXPECT_EQ(manager.getCacheSize(), static_cast<size_t>(1));
        EXPECT_TRUE(manager.getDataBlock(block1_hash).has_value());
        EXPECT_FALSE(manager.getDataBlock(block2_hash).has_value());
        
        manager.addDataBlock(block2, block2_hash);
        EXPECT_EQ(manager.getBlockOrder(), (std::list<size_t>{block2_hash, block1_hash}));
        manager.addDataBlock(block3, block3_hash);
        
        EXPECT_EQ(manager.getCacheSize(), static_cast<size_t>(3));
        EXPECT_EQ(manager.getCacheDump(), final_cache_dump);
        EXPECT_EQ(manager.getBlockOrder(), (std::list<size_t>{block3_hash, block2_hash, block1_hash}));
        EXPECT_EQ(manager.getCacheDump(), final_cache_dump);


        manager.addDataBlock(block1, block1_hash);
        EXPECT_EQ(manager.getBlockOrder(), (std::list<size_t>{block1_hash, block3_hash, block2_hash}));
        EXPECT_EQ(manager.getCacheSize(), static_cast<size_t>(3));
    }
}

TEST(BufferManagerHappyTests, RemoveDataBlockTest){
    BufferManager manager;
    const std::string str1 = "This is test string1", str2 = "And this is test string 2";
    DataBlock block1, block2, block3;
    const size_t block1_hash = 1111, block2_hash = 2222, block3_hash = 3333;

    block1.data_size = str1.size();
    strcpy(block1.data, str1.data());

    block2.data_size = str2.size();
    strcpy(block2.data, str2.data());

    block3.data_size = 3;
    block3.data[0] = 'h';
    block3.data[1] = 'i';
    block3.data[2] = '!';

    manager.addDataBlock(block1, block1_hash);
    manager.addDataBlock(block2, block2_hash);
    manager.addDataBlock(block3, block3_hash);

    manager.removeDataBlock(block2_hash);
    EXPECT_EQ(manager.getCacheSize(), static_cast<size_t>(2));
    EXPECT_EQ(manager.getBlockOrder(), (std::list<size_t>{block3_hash, block1_hash}));
    EXPECT_EQ(manager.getCacheDump(), (std::unordered_map<size_t, DataBlock>{{block1_hash, block1}, {block3_hash, block3}}));

    manager.removeDataBlock(block1_hash);
    EXPECT_FALSE(manager.getDataBlock(block1_hash).has_value());
    EXPECT_FALSE(manager.getDataBlock(block2_hash).has_value());
    EXPECT_TRUE(manager.getDataBlock(block3_hash).has_value());
    manager.removeDataBlock(block3_hash);
    EXPECT_FALSE(manager.getDataBlock(block3_hash).has_value());

    EXPECT_TRUE(manager.getBlockOrder().empty());
    EXPECT_TRUE(manager.getCacheDump().empty());
    EXPECT_EQ(manager.getCacheSize(), static_cast<size_t>(0));
}



TEST(BufferManagerUnhappyTests, RemoveDataBlockOnEmptyBufferTest){
    BufferManager manager;
    DataBlock test_block;
    const size_t test_block_hash = 111111;
    test_block.data_size = 3;

    EXPECT_NO_THROW(manager.removeDataBlock(21314142));
    EXPECT_TRUE(manager.getBlockOrder().empty());

    manager.addDataBlock(test_block, test_block_hash);
    EXPECT_TRUE(manager.getDataBlock(test_block_hash).has_value());

    EXPECT_NO_THROW(manager.removeDataBlock(test_block_hash));
    EXPECT_TRUE(manager.getBlockOrder().empty());
}

