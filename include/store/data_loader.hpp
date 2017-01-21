#ifndef DATA_LOADER_HPP
#define DATA_LOADER_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "db/db.hpp"
#include "encode/encoder.hpp"

using namespace std;

namespace store {
    class DataLoader {
    public:
        DataLoader(boost::shared_ptr<db::DB> db,
                   boost::shared_ptr<encode::Encoder> encoder)
          : db_(db), encoder_(encoder) {
            iter_ = db->new_iterator();
        }

        ~DataLoader() {}

        string get(const string& key) {
            if (iter_->supports_seek()) {
                iter_->seek(key);
                if (iter_->valid()) {
                    string value = iter_->value();
                    string data;
                    if (encoder_->decode(value, data)) {
                        return data;
                    }
                }
            }
            return string();
        }

    private:
        boost::shared_ptr<db::DB> db_;
        boost::shared_ptr<db::Iterator> iter_;
        boost::shared_ptr<encode::Encoder> encoder_;
    };
}

#endif // DATA_LOADER_HPP
