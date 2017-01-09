#ifndef DB_ROCKSDB_HPP
#define DB_ROCKSDB_HPP

#include <string>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>

#include "db.hpp"

namespace db {
    class RocksDBIterator : public Iterator {
    public:
        explicit RocksDBIterator(rocksdb::Iterator* iter,
                                 boost::shared_ptr<encode::Encoder> encoder)
            : iter_(iter), Iterator(encoder) {
            seek_to_first();
        }

        ~RocksDBIterator() {
            delete iter_;
        }

        virtual void seek_to_first() {
            iter_->SeekToFirst();
        }

        virtual void next() {
            iter_->Next();
        }

        virtual string key() {
            return iter_->key().ToString();
        }

        virtual string value() {
            return iter_->value().ToString();
        }

        virtual bool valid() {
            return iter_->Valid();
        }

    private:
        rocksdb::Iterator* iter_;
    };

    class RocksDBWriter : public Writer {
    public:
        explicit RocksDBWriter(rocksdb::DB* db,
                               boost::shared_ptr<encode::Encoder> encoder)
            : db_(db), Writer(encoder) {
            CHECK_NOTNULL(db_);
        }

        virtual void put(const string& key, const string& value) {
            batch_.Put(key, value);
        }

        virtual void del(const string& key) {
            batch_.Delete(key);
        }

        virtual void flush() {
            rocksdb::Status status = db_->Write(rocksdb::WriteOptions(), &batch_);
            CHECK(status.ok()) << "Failed to write batch to rocksdb"
                               << std::endl << status.ToString();
        }

    private:
        rocksdb::DB* db_;
        rocksdb::WriteBatch batch_;
    };

    class RocksDB : public DB {
    public:
        RocksDB(boost::shared_ptr<encode::Encoder> encoder)
            : db_(NULL), DB(encoder) {}

        virtual ~RocksDB() {
            close();
        }

        virtual void open(const string& source, Mode mode);

        virtual void close() {
            if (db_ != NULL) {
                delete db_;
                db_ = NULL;
            }
        }

        virtual string get(const string& key) {
            string value;
            rocksdb::Status s = db_->Get(rocksdb::ReadOptions(), key, &value);
            if (s.ok()) {
                return value;
            }
            return NULL;
        }

        virtual void put(const string& key, const string& value) {
            db_->Put(rocksdb::WriteOptions(), key, value);
        }

        virtual void del(const string& key) {
            db_->Delete(rocksdb::WriteOptions(), key);
        }

        virtual RocksDBIterator* new_iterator() {
            return new RocksDBIterator(db_->NewIterator(rocksdb::ReadOptions()),
                                       encoder());
        }

        virtual RocksDBWriter* new_writer() {
            return new RocksDBWriter(db_, encoder());
        }

    private:
        rocksdb::DB* db_;
    };
} // namespace db

#endif // DB_ROCKSDB_HPP
