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
    boost::shared_ptr<DB> open_db(const string& source, Mode mode) {
        size_t found = source.find(":");
        string backend = "filedb";
        string path = source;

        if (found != string::npos) {
            backend = source.substr(0, found);
            path = source.substr(found+1);
        }

        boost::shared_ptr<DB> db;
        if (backend == "filedb") {
            db.reset(new FileDB(path, mode));
        }

#ifdef WITH_LEVELDB
        if (backend == "leveldb") {
            db.reset(new LevelDB(path, mode));
        }
#endif

#ifdef WITH_LMDB
        if (backend == "lmdb") {
            db.reset(new LMDB(path, mode));
        }
#endif

#ifdef WITH_ROCKSDB
        if (backend == "rocksdb") {
            db.reset(new RocksDB(path, mode));
        }
#endif
        
        if (!db && backend != "filedb") {
            LOG(ERROR) << "Unsupported db backend: " << backend;
        }

        return db;
    }
} // namespace db
