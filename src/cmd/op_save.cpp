#include "cmd/op_save.hpp"
#include "base64/base64.h"
#include "misc/map.hpp"

namespace op {
    bool SaveOP::init(const map<string, string>& config) {
        string db_url = map_get(config, "db");
        if (!db_url.empty()) {
            db_ = db::open_db(db_url, db::WRITE);
            if (db_) {
                writer_ = db_->new_writer();
            }
        }

        get_string_value(config, "key", key_fno_, key_);

        return is_init();
    }

    bool SaveOP::is_init() {
        return writer_ != NULL;
    }

    void SaveOP::flush() {
        if (is_init()) {
            writer_->flush();
            delete writer_;
            writer_ = db_->new_writer();
        }
    }

    void SaveOP::finish() {
        if (writer_) {
            delete writer_;
            writer_ = NULL;
        }

        if (db_) {
            db_->close();
            delete db_;
            db_ = NULL;
        }
    }

    cv::Mat SaveOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        if (!is_init()) {
            LOG(ERROR) << "Failed to save image: NULL db";
        } else {
            string key = key_;
            if (key_fno_ > 0 && key_fno_ <= fields.size()) {
                key = fields[key_fno_ - 1];
            }

            if (key.empty()) {
                LOG(ERROR) << "No save key for image: " << get_key(fields);
            } else {
                std::size_t last_dot = key.rfind(".");
                string ext;
                if (last_dot != string::npos) {
                    ext = key.substr(last_dot);
                }
                vector<unsigned char> img_content;
                cv::imencode(ext, img, img_content);
                writer_->put(key, img_content);
            }
        }

        return img;
    }
}
