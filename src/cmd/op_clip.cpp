#include "cmd/op_clip.hpp"
#include "misc/map.hpp"

namespace op {
    bool ClipOP::init(const map<string, string>& config) {
        get_value(config, "x", x_fno_, x_);
        get_value(config, "y", y_fno_, y_);
        get_value(config, "w", w_fno_, w_);
        get_value(config, "h", h_fno_, h_);

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
            int x = get_field_value(fields, x_fno_, x_);
            int y = get_field_value(fields, y_fno_, y_);
            int w = get_field_value(fields, w_fno_, w_);
            int h = get_field_value(fields, h_fno_, h_);

            if (x < 0 || y < 0 || w <=0 || h <= 0) {
                LOG(ERROR) << "Invalid parameters: " << get_key(fields);
            } else {
                ret = img(cv::Rect(x, y, w, h));
            }
        }

        return ret;
    }
}
