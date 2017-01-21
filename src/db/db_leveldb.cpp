#include "db/db_leveldb.hpp"

#include <string>

namespace db {
    LevelDB::LevelDB(const string& source, Mode mode) : DB(source, mode) {
        leveldb::Options options;
        options.block_size = 65536;
        options.write_buffer_size = 268435456;
        options.max_open_files = 100;
        options.error_if_exists = mode == NEW;
        options.create_if_missing = mode != READ;
        leveldb::DB* db;
        leveldb::Status status = leveldb::DB::Open(options, source, &db);
        CHECK(status.ok()) << "Failed to open leveldb " << source
                           << std::endl << status.ToString();
        db_.reset(db);
        LOG(INFO) << "Opened leveldb " << source;
    }
} // namespace db
