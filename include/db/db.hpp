#ifndef DB_HPP
#define DB_HPP

#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <glog/logging.h>

using namespace std;

namespace db {
    enum Mode { READ, WRITE, NEW };

    class Iterator {
    public:
        Iterator() {}
        virtual ~Iterator() {}
        virtual void seek(const string& key) = 0;
        virtual bool supports_seek() { return false; }
        virtual void seek_to_first() = 0;
        virtual void next() = 0;
        virtual string key() = 0;
        virtual string value() = 0; 
        virtual bool valid() = 0;
    };

    class Writer {
    public:
        Writer() {}
        virtual ~Writer() {}
        virtual void put(const string& key, const string& value) = 0;
        virtual bool del(const string& key) = 0;
        virtual void flush() = 0;
    };

    class DB {
    public:
        DB(const string& source, Mode mode) : mode_(mode) {}
        virtual ~DB() {}
        virtual void close() = 0;

        virtual boost::shared_ptr<Iterator> new_iterator() = 0;
        virtual boost::shared_ptr<Writer> new_writer() = 0;

    protected:
        Mode mode_;
    };

    boost::shared_ptr<DB> open_db(const string& source, Mode mode);
} // namespace db

#endif // DB_HPP
