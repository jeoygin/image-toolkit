#include "cmd/op_blur.hpp"
#include "misc/map.hpp"

namespace op {
    bool BlurOP::init(const map<string, string>& config) {
        get_int_value(config, "kernel", kernel_fno_, kernel_);

        return is_init();
    }

    bool BlurOP::is_init() {
        return true;
    }

    cv::Mat BlurOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "BlurOp is not initialized";
        } else {
            int kernel = get_field_value(fields, kernel_fno_, kernel_);
            cv::GaussianBlur(img, ret, cv::Size(kernel, kernel), 0, 0);
        }

        return ret;
    }
}
