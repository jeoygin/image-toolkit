#ifndef DB_HPP
#define DB_HPP

#include <string>
#include <iostream>
#include <glog/logging.h>

#include "base64/base64.h"

using namespace std;

namespace db {
    enum Mode { READ, WRITE, NEW };

    class Iterator {
    public:
        Iterator() {}
        virtual ~Iterator() {}
        virtual void seek_to_first() = 0;
        virtual void next() = 0;
        virtual string key() = 0;
        virtual string value() = 0;
        virtual void value(vector<unsigned char>& value) {
            string str_value = this->value();
            value.clear();
            if (!str_value.empty()) {
                base64_decode(str_value, value);
            }
        }
        virtual bool valid() = 0;
    };

    class Writer {
    public:
        Writer() {}
        virtual ~Writer() {}
        virtual void put(const string& key, const vector<unsigned char>& value) {
            string str_value = base64_encode(value.data(), value.size());
            put(key, str_value);
        }
        virtual void put(const string& key, const string& value) = 0;
        virtual void flush() = 0;
    };

    class DB {
    public:
        DB() {}
        virtual ~DB() {}
        virtual void open(const string& source, Mode mode) = 0;
        virtual void close() = 0;
        virtual string get(const string& key) = 0;

        virtual void get(const string& key, vector<unsigned char>& value) {
            string str_value = get(key);
            value.clear();
            if (!str_value.empty()) {
                base64_decode(str_value, value);
            }
        }

        virtual void put(const string& key, const string& value) = 0;

        virtual void put(const string& key, const vector<unsigned char>& value) {
            string str_value = base64_encode(value.data(), value.size());
            put(key, str_value);
        }

        virtual int copy(const string& key, DB* dst, const string& dst_key,
                         vector<unsigned char>& aux) {
            if (!dst) {
                return -1;
            }

            aux.clear();
            get(key, aux);
            if (aux.empty()) {
                return -2;
            }

            dst->put(dst_key, aux);
            return 0;
        }

        virtual int copy(const string& key, Writer* writer, const string& dst_key,
                         vector<unsigned char>& aux) {
            if (!writer) {
                return -1;
            }

            aux.clear();
            get(key, aux);
            if (aux.empty()) {
                return -2;
            }

            writer->put(dst_key, aux);
            return 0;
        }

        virtual Iterator* new_iterator() = 0;
        virtual Writer* new_writer() = 0;
    };

    DB* get_db(const string& backend);
    DB* open_db(const string& source, Mode mode);
} // namespace db

#endif // DB_HPP
