#include "db/db.hpp"
#include "db/db_filedb.hpp"

#ifdef WITH_LEVELDB
#include "db/db_leveldb.hpp"
#endif

#ifdef WITH_LMDB
#include "db/db_lmdb.hpp"
#endif

#ifdef WITH_ROCKSDB
#include "db/db_rocksdb.hpp"
#endif

#include <string>
#include <iostream>

namespace db {
    DB* get_db(const string& backend) {
#ifdef WITH_LEVELDB
        if (backend == "leveldb") {
            return new LevelDB();
        }
#endif

#ifdef WITH_LMDB
        if (backend == "lmdb") {
            return new LMDB();
        }
#endif

#ifdef WITH_ROCKSDB
        if (backend == "rocksdb") {
            return new RocksDB();
        }
#endif

        if (backend == "filedb") {
            return new FileDB();
        }

        return NULL;
    }

    DB* open_db(const string& source, Mode mode) {
        size_t found = source.find(":");
        string backend = "filedb";
        string path = source;

        if (found != string::npos) {
            backend = source.substr(0, found);
            path = source.substr(found+1);
        }

        DB* db = get_db(backend);
        if (db == NULL && backend != "filedb") {
            LOG(ERROR) << "Unsupported db backend: " << backend;
        }

        if (db != NULL) {
            db->open(path, mode);
        }

        return db;
    }
} // namespace cb
