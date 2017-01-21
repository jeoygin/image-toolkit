#ifndef DB_LMDB_HPP
#define DB_LMDB_HPP

#include <string>

#include <lmdb.h>

#include "db.hpp"

namespace db {
    inline void MDB_CHECK(int mdb_status, string msg = "") {
        CHECK_EQ(mdb_status, MDB_SUCCESS) << mdb_strerror(mdb_status) 
                                          << " " << msg;
    }

    class LMDBIterator : public Iterator {
    public:
        explicit LMDBIterator(MDB_env* mdb_env)
            : mdb_env_(mdb_env), valid_(false) {
            MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, MDB_RDONLY, &mdb_txn_));
            MDB_CHECK(mdb_dbi_open(mdb_txn_, NULL, 0, &mdb_dbi_));
            MDB_CHECK(mdb_cursor_open(mdb_txn_, mdb_dbi_, &mdb_cursor_));
            seek_to_first();
        }

        ~LMDBIterator() {
            mdb_cursor_close(mdb_cursor_);
            mdb_dbi_close(mdb_env_, mdb_dbi_);
            mdb_txn_abort(mdb_txn_);
        }

        void seek(const string& key) {
            if (key.size() == 0) {
                seek_to_first();
                return;
            }

            mdb_key_.mv_size = key.size();
            mdb_key_.mv_data = const_cast<char*>(key.c_str());
            int mdb_status = mdb_cursor_get(mdb_cursor_, &mdb_key_, &mdb_value_,
                                            MDB_SET_RANGE);
            if (mdb_status == MDB_NOTFOUND) {
                valid_ = false;
            } else {
                MDB_CHECK(mdb_status);
                valid_ = true;
            }
        }

        bool supports_seek() {
            return true;
        }

        void seek_to_first() {
            seek_LMDB(MDB_FIRST);
        }

        void next() {
            seek_LMDB(MDB_NEXT);
        }

        string key() {
            return string(static_cast<const char*>(mdb_key_.mv_data),
                          mdb_key_.mv_size);
        }

        string value() {
            return string(static_cast<const char*>(mdb_value_.mv_data),
                          mdb_value_.mv_size);
        }

        virtual bool valid() {
            return valid_;
        }

    private:
        void seek_LMDB(MDB_cursor_op op) {
            int mdb_status = mdb_cursor_get(mdb_cursor_, &mdb_key_,
                                            &mdb_value_, op);
            if (mdb_status == MDB_NOTFOUND) {
                valid_ = false;
            } else {
                MDB_CHECK(mdb_status);
                valid_ = true;
            }
        }

        MDB_env* mdb_env_;
        MDB_txn* mdb_txn_;
        MDB_dbi mdb_dbi_;
        MDB_cursor* mdb_cursor_;
        MDB_val mdb_key_, mdb_value_;
        bool valid_;
    };

    class LMDBWriter : public Writer {
    public:
        explicit LMDBWriter(MDB_env* mdb_env)
            : mdb_env_(mdb_env) {
            MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, 0, &mdb_txn_));
            MDB_CHECK(mdb_dbi_open(mdb_txn_, NULL, 0, &mdb_dbi_));
        }

        ~LMDBWriter() {
            MDB_CHECK(mdb_txn_commit(mdb_txn_));
            mdb_dbi_close(mdb_env_, mdb_dbi_);
        }

        void put(const string& key, const string& value);

        bool del(const string& key);

        void flush() {
            MDB_CHECK(mdb_txn_commit(mdb_txn_));
            mdb_dbi_close(mdb_env_, mdb_dbi_);
            MDB_CHECK(mdb_txn_begin(mdb_env_, NULL, 0, &mdb_txn_));
            MDB_CHECK(mdb_dbi_open(mdb_txn_, NULL, 0, &mdb_dbi_));
        }

    private:
        MDB_env* mdb_env_;
        MDB_dbi mdb_dbi_;
        MDB_txn* mdb_txn_;
    };

    class LMDBReader {
    public:
        explicit LMDBReader(MDB_dbi* mdb_dbi, MDB_txn* mdb_txn)
            : mdb_dbi_(mdb_dbi), mdb_txn_(mdb_txn) {}

        ~LMDBReader() {}

        string get(const string& key);
    private:
        MDB_env* mdb_env_;
        MDB_dbi* mdb_dbi_;
        MDB_txn* mdb_txn_;
    };

    class LMDB : public DB {
    public:
        LMDB(const string& source, Mode mode);

        ~LMDB() {
            close();
        }

        void open(const string& source, Mode mode);

        void close() {
            if (mdb_env_ != NULL) {
                mdb_env_close(mdb_env_);
                mdb_env_ = NULL;
            }
        }

        boost::shared_ptr<Iterator> new_iterator() {
            return boost::shared_ptr<LMDBIterator>(new LMDBIterator(mdb_env_));
        }

        boost::shared_ptr<Writer> new_writer() {
            return boost::shared_ptr<LMDBWriter>(new LMDBWriter(mdb_env_));
        }


    private:
        MDB_env* mdb_env_;
    };
} // namespace db

#endif // DB_LMDB_HPP
