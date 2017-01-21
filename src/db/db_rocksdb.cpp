#include "db/db_rocksdb.hpp"

#include <string>
#include "rocksdb/utilities/leveldb_options.h"

namespace db {
    RocksDB::RocksDB(const string& source, Mode mode) : DB(source, mode) {
        rocksdb::LevelDBOptions options;
        options.block_size = 65536;
        options.write_buffer_size = 268435456;
        options.max_open_files = 100;
        options.error_if_exists = mode == NEW;
        options.create_if_missing = mode != READ;
        rocksdb::Options rocksdb_options = rocksdb::ConvertOptions(options);

        rocksdb::DB* db;
        rocksdb::Status status = rocksdb::DB::Open(rocksdb_options, source, &db);
        CHECK(status.ok()) << "Failed to open rocksdb " << source
                           << std::endl << status.ToString();
        db_.reset(db);
        LOG(INFO) << "Opened rocksdb " << source;
    }
} // namespace db
