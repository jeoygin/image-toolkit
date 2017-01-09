#ifndef DB_FILEDB_HPP
#define DB_FILEDB_HPP

#include <string>
#include <fstream>

#include "db.hpp"

namespace db {
    class FileDBIterator : public Iterator {
    public:
        FileDBIterator(const string& root,
                       boost::shared_ptr<encode::Encoder> encoder);

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
        char* data_ = NULL;

        void delete_data() {
            if (data_ != NULL) {
                delete data_;
                data_ = NULL;
            }
        }
    };

    class FileDBWriter : public Writer {
    public:
        explicit FileDBWriter(const string& root,
                              boost::shared_ptr<encode::Encoder> encoder)
            : root_(root), Writer(encoder) {}

        virtual void put(const string& key, const vector<unsigned char>& value);

        virtual void put(const string& key, const string& value);

        virtual void del(const string& key);

        virtual void flush() {}

    private:
        string root_;
    };

    class FileDB : public DB {
    public:
        FileDB(boost::shared_ptr<encode::Encoder> encoder) : DB(encoder) {}

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

        virtual void del(const string& key);

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
            return new FileDBIterator(root_, encoder());
        }

        virtual FileDBWriter* new_writer() {
            return new FileDBWriter(root_, encoder());
        }

    private:
        string root_;
    };
} // namespace db

#endif // DB_LEVELDB_HPP
