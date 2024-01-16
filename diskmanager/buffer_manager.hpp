#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>
#include <functional>
#include <optional>
#include <list>

#include "include/duckdb.hpp"
#include "common.hpp"

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

class BufferManager{
public:
    BufferManager() noexcept = default;

public:
    // Get a block of data by its hash. If none is found, the method returns `std::nullopt`.
    std::optional<std::reference_wrapper<const DataBlock>> getDataBlock(const size_t block_hash) noexcept;

    // Removes the data block, identifiable by its `block_hash`, from the cache.
    void removeDataBlock(const size_t block_hash) noexcept;

    /** Add a new data block to the cache.
     * @param[in] data_block DataBlock object
     * @param[in] data_hash hash of the new data block
    */
    void addDataBlock(const DataBlock& data_block, const size_t data_hash) noexcept;

    void clearBuffer() noexcept;

public:
    // Return a list where the data blocks are stored in the use-recency order.
    const std::list<size_t>& getBlockOrder() const noexcept;

    // Return all cached data blocks.
    const std::unordered_map<size_t, DataBlock>& getCacheDump() const noexcept;

    size_t getCacheSize() const noexcept;

private:
    // Puts the data block to the top of the block order list by its data hash.
    void pinBlock(const size_t block_hash) noexcept;
    
    // Removes the block from the cache by its data hash.
    void unpinBlock(const size_t block_hash) noexcept;

    // Remove the least recently used block from the cache.
    void deleteLeastRecentlyUsedBlock() noexcept;

private:

    std::list<size_t> blockhashes_order_;                       /* Keeps the used recency order for data blocks */
    std::unordered_map<size_t, DataBlock> blockhash_to_data_;   /* Stores hashes-to-datablock key pairs */
};