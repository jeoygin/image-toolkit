#ifndef DB_LEVELDB_HPP
#define DB_LEVELDB_HPP

#include <string>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

#include "db.hpp"

namespace db {
    class LevelDBIterator : public Iterator {
    public:
        explicit LevelDBIterator(leveldb::Iterator* iter)
            : iter_(iter) {
            seek_to_first();
        }

        ~LevelDBIterator() {
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
        leveldb::Iterator* iter_;
    };

    class LevelDBWriter : public Writer {
    public:
        explicit LevelDBWriter(leveldb::DB* db) : db_(db) {
            CHECK_NOTNULL(db_);
        }

        virtual void put(const string& key, const string& value) {
            batch_.Put(key, value);
        }

        virtual void flush() {
            leveldb::Status status = db_->Write(leveldb::WriteOptions(), &batch_);
            CHECK(status.ok()) << "Failed to write batch to leveldb"
                               << std::endl << status.ToString();
        }

    private:
        leveldb::DB* db_;
        leveldb::WriteBatch batch_;
    };

    class LevelDB : public DB {
    public:
        LevelDB() : db_(NULL) {}

        virtual ~LevelDB() {
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
            leveldb::Status s = db_->Get(leveldb::ReadOptions(), key, &value);
            if (s.ok()) {
                return value;
            }
            return NULL;
        }

        virtual void put(const string& key, const string& value) {
            db_->Put(leveldb::WriteOptions(), key, value);
        }

        virtual LevelDBIterator* new_iterator() {
            return new LevelDBIterator(db_->NewIterator(leveldb::ReadOptions()));
        }

        virtual LevelDBWriter* new_writer() {
            return new LevelDBWriter(db_);
        }

    private:
        leveldb::DB* db_;
    };
} // namespace db

#endif // DB_LEVELDB_HPP
