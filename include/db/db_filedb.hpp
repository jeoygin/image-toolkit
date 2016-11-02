#ifndef DB_FILEDB_HPP
#define DB_FILEDB_HPP

#include <string>
#include <fstream>

#include "db.hpp"
#include "base64/base64.h"

namespace db {
    class FileDBIterator : public Iterator {
    public:
        FileDBIterator(const string& root);

        ~FileDBIterator() {
            delete_data();
        }

        virtual void seek_to_first() {
            iter_ = files_.begin();
        }

        virtual void next() {
            iter_++;
        }

        virtual string key() {
            return *iter_;
        }

        virtual string value();
        virtual void value(vector<unsigned char>& value);

        virtual bool valid() {
            return iter_ < files_.end();
        }

    private:
        string root_;
        vector<string> files_;
        vector<string>::iterator iter_;
        char* data_;

        void delete_data() {
            if (data_ != NULL) {
                delete data_;
                data_ = NULL;
            }
        }
    };

    class FileDBWriter : public Writer {
    public:
        explicit FileDBWriter(const string& root) : root_(root) {}

        virtual void put(const string& key, const vector<unsigned char>& value);

        virtual void put(const string& key, const string& value);

        virtual void flush() {}

    private:
        string root_;
    };

    class FileDB : public DB {
    public:
        FileDB() {}

        virtual ~FileDB() {
            close();
        }

        virtual void open(const string& source, Mode mode);

        virtual void close() {
        }

        virtual string get(const string& key);

        virtual void get(const string& key, vector<unsigned char>& value);

        virtual void put(const string& key, const vector<unsigned char>& value);

        virtual void put(const string& key, const string& value);

        int copy(const string& key, DB* dst, const string& dst_key,
                 vector<unsigned char>& aux) {
            if (!dst) {
                return -1;
            }

            get(key, aux);
            if (aux.empty()) {
                return -2;
            }

            dst->put(dst_key, aux);
            return 0;
        }

        int copy(const string& key, Writer* writer, const string& dst_key,
                 vector<unsigned char>& aux) {
            if (!writer) {
                return -1;
            }

            get(key, aux);
            if (aux.empty()) {
                return -2;
            }

            writer->put(dst_key, aux);
            return 0;
        }

        virtual FileDBIterator* new_iterator() {
            return new FileDBIterator(root_);
        }

        virtual FileDBWriter* new_writer() {
            return new FileDBWriter(root_);
        }

    private:
        string root_;
    };
} // namespace db

#endif // DB_LEVELDB_HPP
