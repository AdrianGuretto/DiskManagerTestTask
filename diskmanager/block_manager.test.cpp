#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "include/duckdb.hpp"
#include "block_manager.hpp"

using namespace std::filesystem;
using namespace std::string_literals;

std::filesystem::path operator""_p(const char* str, const size_t str_size){
    return std::filesystem::path(str);
}

class BlockManagerFilesystemTests : public testing::Test{
protected:
    static void SetUpTestSuite(){
        // Create the test directory
        create_directory(test_dir_path_);

        // Fill the test directory with files
        std::ofstream test_file1(test_db_file_path1_, std::ios::binary | std::ios::trunc);
        if (!test_file1.is_open()){
            throw std::runtime_error("Failed to set up BlockManagerFilesystemTests: failed to create test_file_1"s);
        }
        test_file1.close();

        std::ofstream test_file2(test_db_file_path2_, std::ios::binary);
        if (!test_file2.is_open()){
            throw std::runtime_error("Failed to set up BlockManagerFilesystemTests: failed to create test_file_2"s);
        }

        const std::string test_str1("This is test string number one");
        const std::string test_str2("And this is the second test string");
        const std::string test_str3("Another string under number three");
        const std::string test_str4("Okay, the last data in string");

        test_block1_.data_size = test_str1.size();
        test_block2_.data_size = test_str2.size();
        test_block3_.data_size = test_str3.size();
        test_block4_.data_size = test_str4.size();

        std::memcpy(test_block1_.data, test_str1.data(), test_str1.size());
        std::memcpy(test_block2_.data, test_str2.data(), test_str2.size());
        std::memcpy(test_block3_.data, test_str3.data(), test_str3.size());
        std::memcpy(test_block4_.data, test_str4.data(), test_str4.size());

    }

    static void TearDownTestSuite(){
        remove_all(test_dir_path_);
    }

    void TearDown() override{
        // Clear the test files
        std::ofstream test_file1(test_db_file_path1_, std::ios::binary | std::ios::trunc);
        if (!test_file1.is_open()){
            throw std::runtime_error("Failed to set up a test for BlockManagerFilesystemTests fixture: failed to open test_file_1"s);
        }
        test_file1.close();

        std::ofstream test_file2(test_db_file_path2_, std::ios::binary | std::ios::trunc);
        if (!test_file2.is_open()){
            throw std::runtime_error("Failed to set up a test for BlockManagerFilesystemTests fixture: failed to open test_file_2"s);
        }
    }


    static path test_dir_path_;
    static path test_db_file_path1_;
    static path test_db_file_path2_;

    static DataBlock test_block1_;
    static DataBlock test_block2_;
    static DataBlock test_block3_;
    static DataBlock test_block4_;
};

path BlockManagerFilesystemTests::test_dir_path_ = std::filesystem::temp_directory_path() / "block_manager_test_tmp_dir"_p;
path BlockManagerFilesystemTests::test_db_file_path1_ = test_dir_path_ / "test_db_file_1.csv"_p;
path BlockManagerFilesystemTests::test_db_file_path2_ = test_dir_path_ / "test_db_file_2.csv"_p;

DataBlock BlockManagerFilesystemTests::test_block1_;
DataBlock BlockManagerFilesystemTests::test_block2_;
DataBlock BlockManagerFilesystemTests::test_block3_;
DataBlock BlockManagerFilesystemTests::test_block4_;

TEST_F(BlockManagerFilesystemTests, BlockManagerInitStateTest){
    duckdb::DuckDB db(test_db_file_path1_.generic_string());
    BlockManager bmanager(db);

    EXPECT_EQ(bmanager.getBufferSize(), static_cast<size_t>(0));
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(0));
    EXPECT_EQ(bmanager.getTotalWrittenBlocksCount(), static_cast<size_t>(0));
}

TEST_F(BlockManagerFilesystemTests, BlockManagerWriteBlockTest){
    duckdb::DuckDB db(test_db_file_path1_.generic_string());
    BlockManager bmanager(db);
    bmanager.writeBlock(test_block1_.data, test_block1_.data_size);

    EXPECT_EQ(bmanager.getBufferSize(), static_cast<size_t>(1));
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(0));
    EXPECT_EQ(bmanager.getTotalWrittenBlocksCount(), static_cast<size_t>(1));

    bmanager.writeBlock(test_block1_.data, test_block1_.data_size);
    EXPECT_EQ(bmanager.getBufferSize(), static_cast<size_t>(1));
    EXPECT_EQ(bmanager.getTotalWrittenBlocksCount(), static_cast<size_t>(1));

    bmanager.writeBlock(test_block2_.data, test_block2_.data_size);
    EXPECT_EQ(bmanager.getBufferSize(), static_cast<size_t>(2));
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(2));
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(0));
}

TEST_F(BlockManagerFilesystemTests, BlockManagerReadBlockTest){
    duckdb::DuckDB db(test_db_file_path1_.generic_string());
    BlockManager bmanager(db);

    DataBlock read_block1;
    DataBlock read_block2;
    

    bmanager.writeBlock(test_block1_.data, test_block1_.data_size);
    bmanager.writeBlock(test_block2_.data, test_block2_.data_size);
    EXPECT_EQ(bmanager.getBufferSize(), static_cast<size_t>(2));

    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(0));
    EXPECT_TRUE(bmanager.readBlock(test_block1_.Hash(), read_block1));
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(1));
    EXPECT_EQ(bmanager.getTotalWrittenBlocksCount(), static_cast<size_t>(0));

    EXPECT_FALSE(bmanager.readBlock(321331, read_block1)); // would not read a block with an invalid cache
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(1));
    EXPECT_EQ(read_block1, test_block1_);

    EXPECT_TRUE(bmanager.readBlock(test_block2_.Hash(), read_block2));
    EXPECT_EQ(bmanager.getTotalReadBlocksCount(), static_cast<size_t>(2));
    EXPECT_EQ(bmanager.getBufferSize(), static_cast<size_t>(2));
}