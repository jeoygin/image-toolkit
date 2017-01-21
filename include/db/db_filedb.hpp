#ifndef DB_FILEDB_HPP
#define DB_FILEDB_HPP

#include <string>
#include <map>
#include <fstream>

#include "db.hpp"

namespace db {
    class FileDBIterator : public Iterator {
    public:
        FileDBIterator(const string& root);

        ~FileDBIterator() {}

        virtual void seek(const string& key) {
            key_ = key;
            random_ = true;
        }

        virtual bool supports_seek() {
            return true;
        }

        virtual void seek_to_first() {
            if (!inited_) {
                init();
            }
            iter_ = keys_.begin();
        }

        virtual void next() {
            if (!inited_) {
                init();
            }
            if (!random_) {
                iter_++;
            } else {
                std::map<string, int>::iterator it = key_map_.find(key_);
                if (it != key_map_.end()) {
                    iter_ = keys_.begin() + it->second;
                } else {
                    iter_ = keys_.end();
                }
                random_ = false;
            }
        }

        virtual string key() {
            if (!random_ && !inited_) {
                init();
            }
            return !random_ ? *iter_ : key_;
        }

        virtual string value();
        virtual bool valid();


    private:
        string root_;
        bool inited_;
        bool random_;
        vector<string> keys_;
        map<string, int> key_map_;
        vector<string>::iterator iter_;
        string key_;

        virtual void init();
    };

    class FileDBWriter : public Writer {
    public:
        explicit FileDBWriter(const string& root) : root_(root) {}

        virtual void put(const string& key, const string& value);

        virtual bool del(const string& key);

        virtual void flush() {}

    private:
        string root_;
    };

    class FileDB : public DB {
    public:
        FileDB(const string& source, Mode mode) : DB(source, mode), root_(source) {}

        virtual ~FileDB() {
            close();
        }

        virtual void close() {
        }

        virtual boost::shared_ptr<Iterator> new_iterator() {
            return boost::shared_ptr<FileDBIterator>(new FileDBIterator(root_));
        }

        virtual boost::shared_ptr<Writer> new_writer() {
            return boost::shared_ptr<FileDBWriter>(new FileDBWriter(root_));
        }

    private:
        string root_;
    };
} // namespace db

#endif // DB_LEVELDB_HPP
