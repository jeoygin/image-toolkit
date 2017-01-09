#ifndef DB_HPP
#define DB_HPP

#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <glog/logging.h>

#include "encode/encoder.hpp"

using namespace std;

namespace db {
    const boost::shared_ptr<encode::Encoder> DEFAULT_ENCODER = encode::get_encoder("base64");

    enum Mode { READ, WRITE, NEW };

    class Iterator {
    public:
        Iterator(boost::shared_ptr<encode::Encoder> encoder)
            : _encoder(encoder) {}
        virtual ~Iterator() {}
        virtual void seek_to_first() = 0;
        virtual void next() = 0;
        virtual string key() = 0;
        virtual string value() = 0;
        virtual void value(vector<unsigned char>& value) {
            string str_value = this->value();
            value.clear();
            if (!str_value.empty()) {
                _encoder->decode(str_value, value);
            }
        }
        virtual bool valid() = 0;

        boost::shared_ptr<encode::Encoder> encoder() {
            return _encoder;
        }

        void encoder(boost::shared_ptr<encode::Encoder> encoder) {
            _encoder = encoder;
        }

    private:
        boost::shared_ptr<encode::Encoder> _encoder;
    };

    class Writer {
    public:
        Writer(boost::shared_ptr<encode::Encoder> encoder)
            : _encoder(encoder) {}
        virtual ~Writer() {}
        virtual void put(const string& key, const vector<unsigned char>& value) {
            string str_value = _encoder->encode(value);
            put(key, str_value);
        }
        virtual void put(const string& key, const string& value) = 0;
        virtual void del(const string& key) = 0;
        virtual void flush() = 0;

        boost::shared_ptr<encode::Encoder> encoder() {
            return _encoder;
        }

        void encoder(boost::shared_ptr<encode::Encoder> encoder) {
            _encoder = encoder;
        }

    private:
        boost::shared_ptr<encode::Encoder> _encoder;
    };

    class DB {
    public:
        DB(boost::shared_ptr<encode::Encoder> encoder = DEFAULT_ENCODER)
            : _encoder(encoder) {}
        virtual ~DB() {}
        virtual void open(const string& source, Mode mode) = 0;
        virtual void close() = 0;
        virtual string get(const string& key) = 0;

        virtual void get(const string& key, vector<unsigned char>& value) {
            string str_value = get(key);
            value.clear();
            if (!str_value.empty()) {
                encoder()->decode(str_value, value);
            }
        }

        virtual void put(const string& key, const string& value) = 0;

        virtual void put(const string& key, const vector<unsigned char>& value) {
            string str_value = encoder()->encode(value);
            put(key, str_value);
        }

        virtual void del(const string& key) = 0;

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

        boost::shared_ptr<encode::Encoder> encoder() {
            return _encoder;
        }

        void encoder(boost::shared_ptr<encode::Encoder> encoder) {
            _encoder = encoder;
        }

    private:
        boost::shared_ptr<encode::Encoder> _encoder;
    };

    DB* get_db(const string& backend,
               boost::shared_ptr<encode::Encoder> encoder = DEFAULT_ENCODER);
    DB* open_db(const string& source, Mode mode,
                boost::shared_ptr<encode::Encoder> encoder = DEFAULT_ENCODER);
} // namespace db

#endif // DB_HPP
