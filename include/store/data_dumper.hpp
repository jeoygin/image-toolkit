#ifndef DATA_DUMPER_HPP
#define DATA_DUMPER_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "db/db.hpp"
#include "encode/encoder.hpp"

using namespace std;

namespace store {
    class DataDumper {
    public:
        DataDumper(boost::shared_ptr<db::DB> db,
                   boost::shared_ptr<encode::Encoder> encoder)
          : db_(db), encoder_(encoder) {
            writer_ = db->new_writer();
        }

        ~DataDumper() {
            flush();
        }

        void put(const string& key, const string& value) {
            writer_->put(key, encoder_->encode(value));
        }

        bool del(const string& key) {
            return writer_->del(key);
        }

        void flush() {
            writer_->flush();
        }        

    private:
        boost::shared_ptr<db::DB> db_;
        boost::shared_ptr<db::Writer> writer_;
        boost::shared_ptr<encode::Encoder> encoder_;
    };
}

#endif // DATA_DUMPER_HPP
