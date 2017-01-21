#include "db/db_lmdb.hpp"
#include "fs/fs.hpp"

#include <boost/scoped_ptr.hpp>

#include <sys/stat.h>

namespace db {
    const size_t LMDB_MAP_SIZE = 1099511627776;  // 1 TB

    LMDB::LMDB(const string& source, Mode mode) : DB(source, mode) {
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

    void LMDBWriter::put(const string& key, const string& value) {
        MDB_val mdb_key, mdb_value;
        mdb_key.mv_data = const_cast<char*>(key.data());
        mdb_key.mv_size = key.size();
        mdb_value.mv_data = const_cast<char*>(value.data());
        mdb_value.mv_size = value.size();
        MDB_CHECK(mdb_put(mdb_txn_, mdb_dbi_, &mdb_key, &mdb_value, 0));
    }

    bool LMDBWriter::del(const string& key) {
        MDB_val mdb_key;
        mdb_key.mv_data = const_cast<char*>(key.data());
        mdb_key.mv_size = key.size();

        int mdb_status = mdb_del(mdb_txn_, mdb_dbi_, &mdb_key, NULL);
        if (mdb_status == MDB_SUCCESS) {
            return true;
        } else if (mdb_status == MDB_NOTFOUND) {
            return false;
        }
        MDB_CHECK(mdb_status, key);
        return false;
    }
 } // namespace db
