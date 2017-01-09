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
    DB* get_db(const string& backend,
               boost::shared_ptr<encode::Encoder> encoder) {
        DB* db = NULL;

#ifdef WITH_LEVELDB
        if (backend == "leveldb") {
            db = new LevelDB(encoder);
        }
#endif

#ifdef WITH_LMDB
        if (backend == "lmdb") {
            db = new LMDB(encoder);
        }
#endif

#ifdef WITH_ROCKSDB
        if (backend == "rocksdb") {
            db = new RocksDB(encoder);
        }
#endif

        if (backend == "filedb") {
            db = new FileDB(encoder);
        }

        return db;
    }

    DB* open_db(const string& source, Mode mode,
                boost::shared_ptr<encode::Encoder> encoder) {
        size_t found = source.find(":");
        string backend = "filedb";
        string path = source;

        if (found != string::npos) {
            backend = source.substr(0, found);
            path = source.substr(found+1);
        }

        DB* db = get_db(backend, encoder);
        if (db == NULL && backend != "filedb") {
            LOG(ERROR) << "Unsupported db backend: " << backend;
        }

        if (db != NULL) {
            db->open(path, mode);
        }

        return db;
    }
} // namespace cb
