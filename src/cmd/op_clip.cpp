#include "cmd/op_clip.hpp"
#include "misc/map.hpp"

namespace op {
    bool ClipOP::init(const map<string, string>& config) {
        get_int_value(config, "x", x_fno_, x_);
        get_int_value(config, "y", y_fno_, y_);
        get_int_value(config, "w", w_fno_, w_);
        get_int_value(config, "h", h_fno_, h_);

        return is_init();
    }

    bool ClipOP::is_init() {
        return (x_ >= 0 || x_fno_ > 0)
            && (y_ >= 0 || y_fno_ > 0)
            && (w_ > 0 || w_fno_ > 0)
            && (h_ > 0 || h_fno_ > 0);
    }

    cv::Mat ClipOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "ClipOp is not initialized";
        } else {
            int x = x_, y = y_, w = w_, h = h_;

            if (x_fno_ > 0 && x_fno_ <= fields.size()) {
                x = std::stoi(fields[x_fno_ - 1]);
            }

            if (y_fno_ > 0 && y_fno_ <= fields.size()) {
                y = std::stoi(fields[y_fno_ - 1]);
            }

            if (w_fno_ > 0 && w_fno_ <= fields.size()) {
                w = std::stoi(fields[w_fno_ - 1]);
            }

            if (h_fno_ > 0 && h_fno_ <= fields.size()) {
                h = std::stoi(fields[h_fno_ - 1]);
            }

            if (x < 0 || y < 0 || w <=0 || h <= 0) {
                LOG(ERROR) << "Invalid parameters: " << get_key(fields);
            } else {
                ret = img(cv::Rect(x, y, w, h));
            }
        }

        return ret;
    }
}
