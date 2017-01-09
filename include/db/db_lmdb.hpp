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
        explicit LMDBIterator(MDB_txn* mdb_txn, MDB_cursor* mdb_cursor,
                              boost::shared_ptr<encode::Encoder> encoder)
            : mdb_txn_(mdb_txn), mdb_cursor_(mdb_cursor), valid_(false),
              Iterator(encoder) {
            seek_to_first();
        }

        ~LMDBIterator() {
            mdb_cursor_close(mdb_cursor_);
            mdb_txn_abort(mdb_txn_);
        }

        virtual void seek_to_first() {
            seek(MDB_FIRST);
        }

        virtual void next() {
            seek(MDB_NEXT);
        }

        virtual string key() {
            return string(static_cast<const char*>(mdb_key_.mv_data),
                          mdb_key_.mv_size);
        }

        virtual string value() {
            return string(static_cast<const char*>(mdb_value_.mv_data),
                          mdb_value_.mv_size);
        }

        virtual bool valid() {
            return valid_;
        }

    private:
        void seek(MDB_cursor_op op) {
            int mdb_status = mdb_cursor_get(mdb_cursor_, &mdb_key_,
                                            &mdb_value_, op);
            if (mdb_status == MDB_NOTFOUND) {
                valid_ = false;
            } else {
                MDB_CHECK(mdb_status);
                valid_ = true;
            }
        }

        MDB_txn* mdb_txn_;
        MDB_cursor* mdb_cursor_;
        MDB_val mdb_key_, mdb_value_;
        bool valid_;
    };

    class LMDBWriter : public Writer {
    public:
        explicit LMDBWriter(MDB_dbi* mdb_dbi, MDB_txn* mdb_txn,
                            boost::shared_ptr<encode::Encoder> encoder)
            : mdb_dbi_(mdb_dbi), mdb_txn_(mdb_txn), Writer(encoder) {
        }

        virtual void put(const string& key, const string& value);

        virtual bool del(const string& key);

        virtual void flush() {
            MDB_CHECK(mdb_txn_commit(mdb_txn_));
        }

    private:
        MDB_dbi* mdb_dbi_;
        MDB_txn* mdb_txn_;
    };

    class LMDBReader {
    public:
        explicit LMDBReader(MDB_dbi* mdb_dbi, MDB_txn* mdb_txn)
            : mdb_dbi_(mdb_dbi), mdb_txn_(mdb_txn) {}

        virtual ~LMDBReader() {}

        virtual string get(const string& key);
    private:
        MDB_dbi* mdb_dbi_;
        MDB_txn* mdb_txn_;
    };

    class LMDB : public DB {
    public:
        LMDB(boost::shared_ptr<encode::Encoder> encoder)
            : mdb_env_(NULL), reader_(NULL), DB(encoder) {}

        virtual ~LMDB() {
            if (reader_ != NULL) {
                delete reader_;
                reader_ = NULL;
            }
            close();
        }

        virtual void open(const string& source, Mode mode);

        virtual void close() {
            if (mdb_env_ != NULL) {
                mdb_dbi_close(mdb_env_, mdb_dbi_);
                mdb_env_close(mdb_env_);
                mdb_env_ = NULL;
            }
        }

        virtual string get(const string& key);

        virtual void put(const string& key, const string& value);

        virtual bool del(const string& key);

        virtual LMDBIterator* new_iterator();

        virtual LMDBWriter* new_writer();

        virtual LMDBReader* new_reader();

    private:
        MDB_env* mdb_env_;
        MDB_dbi mdb_dbi_;

        LMDBReader* reader_;
    };
} // namespace db

#endif // DB_LMDB_HPP
