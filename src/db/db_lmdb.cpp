#include "db/db_lmdb.hpp"
#include "fs/fs.hpp"

#include <boost/scoped_ptr.hpp>

#include <sys/stat.h>

namespace db {
    const size_t LMDB_MAP_SIZE = 1099511627776;  // 1 TB

    void LMDB::open(const string& source, Mode mode) {
        MDB_CHECK(mdb_env_create(&mdb_env_));
        MDB_CHECK(mdb_env_set_mapsize(mdb_env_, LMDB_MAP_SIZE));
        if (mode != READ) {
            if (!fs::dir_exists(source)) {
                CHECK_EQ(mkdir(source.c_str(), 0744), 0)
                    << "mkdir " << source << " failed";
            }
        }
        int flags = 0;
        if (mode == READ) {
            flags = MDB_RDONLY | MDB_NOTLS;
        }
        int rc = mdb_env_open(mdb_env_, source.c_str(), flags, 0664);
#ifndef ALLOW_LMDB_NOLOCK
        MDB_CHECK(rc);
#else
        if (rc == EACCES) {
            LOG(WARNING) << "Permission denied. Trying with MDB_NOLOCK ...";
            // Close and re-open environment handle
            mdb_env_close(mdb_env_);
            MDB_CHECK(mdb_env_create(&mdb_env_));
            // Try again with MDB_NOLOCK
            flags |= MDB_NOLOCK;
            MDB_CHECK(mdb_env_open(mdb_env_, source.c_str(), flags, 0664));
        } else {
            MDB_CHECK(rc);
        }
#endif
        LOG(INFO) << "Opened lmdb " << source;
    }

    LMDBIterator* LMDB::new_iterator() {
        MDB_txn* mdb_txn;
        MDB_cursor* mdb_cursor;
        MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, MDB_RDONLY, &mdb_txn));
        MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
        MDB_CHECK(mdb_cursor_open(mdb_txn, mdb_dbi_, &mdb_cursor));
        return new LMDBIterator(mdb_txn, mdb_cursor);
    }

    LMDBWriter* LMDB::new_writer() {
        MDB_txn* mdb_txn;
        MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, 0, &mdb_txn));
        MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
        return new LMDBWriter(&mdb_dbi_, mdb_txn);
    }

    LMDBReader* LMDB::new_reader() {
        if (reader_ == NULL) {
            MDB_txn* mdb_txn;
            MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, MDB_RDONLY, &mdb_txn));
            MDB_CHECK(mdb_dbi_open(mdb_txn, NULL, 0, &mdb_dbi_));
            reader_ = new LMDBReader(&mdb_dbi_, mdb_txn);
        }
        return reader_;
    }

    string LMDB::get(const string& key) {
        LMDBReader* reader = new_reader();
        return reader->get(key);
    }

    void LMDB::put(const string& key, const string& value) {
        boost::scoped_ptr<LMDBWriter> writer(new_writer());
        writer->put(key, value);
    }

    void LMDBWriter::put(const string& key, const string& value) {
        MDB_val mdb_key, mdb_value;
        mdb_key.mv_data = const_cast<char*>(key.data());
        mdb_key.mv_size = key.size();
        mdb_value.mv_data = const_cast<char*>(value.data());
        mdb_value.mv_size = value.size();
        MDB_CHECK(mdb_put(mdb_txn_, *mdb_dbi_, &mdb_key, &mdb_value, 0));
    }

    string LMDBReader::get(const string& key) {
        MDB_val mdb_key, mdb_value;
        mdb_key.mv_data = const_cast<char*>(key.data());
        mdb_key.mv_size = key.size();
        MDB_CHECK(mdb_get(mdb_txn_, *mdb_dbi_, &mdb_key, &mdb_value));

        return string(static_cast<const char*>(mdb_value.mv_data),
                      mdb_value.mv_size);
    }
 } // namespace db
