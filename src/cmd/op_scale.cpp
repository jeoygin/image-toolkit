#include "cmd/op_scale.hpp"
#include "misc/map.hpp"

namespace op {
    bool ScaleOP::init(const map<string, string>& config) {
        get_value(config, "width", width_fno_, width_);
        get_value(config, "height", height_fno_, height_);

        return is_init();
    }

    bool ScaleOP::is_init() {
        return true;
    }

    cv::Mat ScaleOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "ScaleOp is not initialized";
        } else {
            int width = get_field_value(fields, width_fno_, width_);
            int height = get_field_value(fields, height_fno_, height_);

            if (width <= 1e-6 || height <= 1e-6) {
                LOG(ERROR) << "Invalid width x height ("
                           << width << "x" << height
                           << "): " << get_key(fields);
            } else {
                cv::resize(img, ret, cv::Size((int)(img.cols * width + 0.5),
                                              (int)(img.rows * height + 0.5)));
            }
        }

        return ret;
    }
}
