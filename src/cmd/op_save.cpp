#include "cmd/op_save.hpp"
#include "encode/encoder.hpp"
#include "misc/map.hpp"

using encode::Encoder;

namespace op {
    bool SaveOP::init(const map<string, string>& config) {
        string db_url = map_get(config, "db");
        string enc_str = map_get(config, "enc");

        if (!db_url.empty()) {
            boost::shared_ptr<Encoder> encoder = encode::get_encoder(enc_str);
            if (!encoder) {
                LOG(ERROR) << "Unsupported encoder: " << enc_str;
                return false;
            }

            auto db = db::open_db(db_url, db::WRITE);
            if (db) {
                dumper_.reset(new store::DataDumper(db, encoder));
            }
        }

        get_value(config, "key", key_fno_, key_);

        return is_init();
    }

    bool SaveOP::is_init() {
        return dumper_ != NULL;
    }

    void SaveOP::flush() {
        if (is_init()) {
            dumper_->flush();
        }
    }

    void SaveOP::finish() {
    }

    cv::Mat SaveOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        if (!is_init()) {
            LOG(ERROR) << "Failed to save image: NULL db";
        } else {
            string key = get_field_value(fields, key_fno_, key_);

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
                dumper_->put(key, string(img_content.begin(), img_content.end()));
            }
        }

        return img;
    }
}
