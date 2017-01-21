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
        explicit RocksDBIterator(rocksdb::DB* db)
            : iter_(db->NewIterator(rocksdb::ReadOptions())) {
            seek_to_first();
        }

        ~RocksDBIterator() {
            delete iter_;
        }

        void seek(const string& key) {
            iter_->Seek(key);
        }

        bool supports_seek() {
            return true;
        }

        void seek_to_first() {
            iter_->SeekToFirst();
        }

        void next() {
            iter_->Next();
        }

        string key() {
            return iter_->key().ToString();
        }

        string value() {
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
        explicit RocksDBWriter(rocksdb::DB* db)
            : db_(db) {
            CHECK_NOTNULL(db_);
            batch_.reset(new rocksdb::WriteBatch());
        }

        ~RocksDBWriter() {
            flush();
        }

        virtual void put(const string& key, const string& value) {
            batch_->Put(key, value);
        }

        virtual bool del(const string& key) {
            batch_->Delete(key);
            return true;
        }

        virtual void flush() {
            rocksdb::Status status = db_->Write(rocksdb::WriteOptions(), batch_.get());
            batch_.reset(new rocksdb::WriteBatch());
            CHECK(status.ok()) << "Failed to write batch to rocksdb"
                               << std::endl << status.ToString();
        }

    private:
        rocksdb::DB* db_;
        boost::shared_ptr<rocksdb::WriteBatch> batch_;
    };

    class RocksDB : public DB {
    public:
        RocksDB(const string& source, Mode mode);

        virtual ~RocksDB() {
            close();
        }

        virtual void close() {
            db_.reset();
        }

        virtual boost::shared_ptr<Iterator> new_iterator() {
            return boost::shared_ptr<RocksDBIterator>(new RocksDBIterator(db_.get()));
        }

        virtual boost::shared_ptr<Writer> new_writer() {
            return boost::shared_ptr<RocksDBWriter>(new RocksDBWriter(db_.get()));
        }

    private:
        boost::shared_ptr<rocksdb::DB> db_;
    };
} // namespace db

#endif // DB_ROCKSDB_HPP
