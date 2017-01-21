#ifndef DB_LEVELDB_HPP
#define DB_LEVELDB_HPP

#include <string>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "db.hpp"

namespace db {
    class LevelDBIterator : public Iterator {
    public:
        explicit LevelDBIterator(leveldb::DB* db)
            : iter_(db->NewIterator(leveldb::ReadOptions())) {
            seek_to_first();
        }

        ~LevelDBIterator() {}

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

        bool valid() {
            return iter_->Valid();
        }

    private:
        boost::shared_ptr<leveldb::Iterator> iter_;
    };

    class LevelDBWriter : public Writer {
    public:
        explicit LevelDBWriter(leveldb::DB* db)
            : db_(db) {
            CHECK_NOTNULL(db_);
            batch_.reset(new leveldb::WriteBatch());
        }

        ~LevelDBWriter() {
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
            leveldb::Status status = db_->Write(leveldb::WriteOptions(), batch_.get());
            batch_.reset(new leveldb::WriteBatch());
            CHECK(status.ok()) << "Failed to write batch to leveldb"
                               << std::endl << status.ToString();
        }

    private:
        leveldb::DB* db_;
        boost::shared_ptr<leveldb::WriteBatch> batch_;
    };

    class LevelDB : public DB {
    public:
        LevelDB(const string& source, Mode mode);

        virtual ~LevelDB() {
            close();
        }

        virtual void close() {
            db_.reset();
        }

        virtual boost::shared_ptr<Iterator> new_iterator() {
            return boost::shared_ptr<LevelDBIterator>(new LevelDBIterator(db_.get()));
        }

        virtual boost::shared_ptr<Writer> new_writer() {
            return boost::shared_ptr<LevelDBWriter>(new LevelDBWriter(db_.get()));
        }

    private:
        boost::shared_ptr<leveldb::DB> db_;
    };
} // namespace db

#endif // DB_LEVELDB_HPP
