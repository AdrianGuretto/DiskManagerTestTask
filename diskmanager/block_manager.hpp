#pragma once

#include "common.hpp"

#include "buffer_manager.hpp"

#include <filesystem>
#include <fstream>

/*
Тестовое задание: Разработка Buffer Manager и Block Manager для работы с диском

Цель: Создать упрощённые версии Buffer Manager и Block Manager, которые могут взаимодействовать с файловой системой для чтения и записи блоков данных.

Требования:

Block Manager:
Разработайте класс BlockManager, который будет отвечать за чтение и запись блоков данных на диск.
Каждый блок данных должен иметь фиксированный размер (например, 4 КБ).
Методы класса должны включать readBlock и writeBlock, которые будут читать и записывать блоки данных по заданному идентификатору блока.
Buffer Manager:
Разработайте класс BufferManager, который будет управлять кэшированием блоков данных в памяти.
Реализуйте стратегию замещения для управления блоками в памяти, например, LRU (Least Recently Used).
Методы класса должны включать pinBlock и unpinBlock, которые будут "закреплять" блоки в памяти и "освобождать" их соответственно.
Интеграция и тестирование:
Обеспечьте интеграцию между BlockManager и BufferManager.
Напишите тесты, демонстрирующие работу менеджеров, включая чтение, запись и кэширование данных.


1. BufferManager
Задачи:
Реализовать управление буфером в памяти для хранения блоков данных.
Реализовать стратегию замещения страниц (например, LRU - Least Recently Used).
Требования:
Интерфейс для запроса и освобождения блоков данных.


2. BlockManager
Задачи:
Управление блоками данных на диске.
Чтение и запись блоков данных.

Требования:
распределение пространства на диске.
Обработка ошибок ввода/вывода.


Referenses:
    https://github.com/cmu-db/bustub
    ttps://github.com/cmu-db/noisepage
    https://github.com/duckdb/duckdb
*/

class BlockManager{
public:
    explicit BlockManager() = default;
    explicit BlockManager(duckdb::DuckDB& db_obj);

public:
    /** Writes data to the currently openned file and caches the value in the buffer.
     * @param[in] data_bytes a pointer to the data buffer000
     * @param[in] data_size a number of bytes to read from the data buffer
    */
    void writeBlock(const char* data_bytes, const size_t data_size);

    /** Reads a data block from the storage file by its data hash.
     * @param[in] data_hash hash for the datablock to read00
     * @param[in] in_block a block object to read a data block to
     * @return `true` if the block existed and has been successfully read, `false` otherwise.
    */ 
    bool readBlock(const size_t data_hash, DataBlock& in_block) noexcept;

    /** Change the current database.
     * @param[in] n_db a reference to a new database object
    */
    void setNewDBObject(duckdb::DuckDB& n_db) noexcept;

public:

    // Get a number of data blocks currently in the buffer.
    size_t getBufferSize() const noexcept;

    // Get a total number of read data blocks.
    size_t getTotalReadBlocksCount() const noexcept;
    
    // Get a total number of written data blocks.
    size_t getTotalWrittenBlocksCount() const noexcept;

    /** Create new data blocks and place the data evenly inside of them
     * @param[in] data a buffer to read the data from.
     * @param[in] data_size number of bytes to read
     * @return `std::vector` with DataBlock objects
    */
    static std::vector<DataBlock> createDataBlocks(const char* data, const size_t data_size);

private:
    /** Inserts a new data block to the database and the buffer.
     * @param[in] dblock DataBlock object
     * @param[in] block_hash a hash of the data block
     * @throw `std::runtime_error` on fail to insert the data to the database.
    */
    void insertDataBlockToDB(const DataBlock& dblock, const size_t block_hash);

private:
    mutable BufferManager buff_manager_;

    std::unique_ptr<duckdb::Connection> conn_db_;

    size_t written_blocks_count_;
    size_t read_blocks_count_;
};