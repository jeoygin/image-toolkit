#include "db/db_rocksdb.hpp"

#include <string>
#include <rocksdb/table.h>

namespace db {
    void RocksDB::open(const string& source, Mode mode) {
        rocksdb::BlockBasedTableOptions table_options;
        table_options.block_size = 8192;
        rocksdb::Options options;
        options.table_factory.reset(rocksdb::NewBlockBasedTableFactory(table_options));
        options.write_buffer_size = 128 * 1024 * 1024;
        options.target_file_size_base = 128 * 1024 * 1024;
        options.max_bytes_for_level_base = 1024 * 1024 * 1024;
        options.max_open_files = 100;
        options.error_if_exists = mode == NEW;
        options.create_if_missing = mode != READ;
        rocksdb::Status status = rocksdb::DB::Open(options, source, &db_);
        CHECK(status.ok()) << "Failed to open rocksdb " << source
                           << std::endl << status.ToString();
        LOG(INFO) << "Opened rocksdb " << source;
    }
} // namespace db
