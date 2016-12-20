#include "cmd/op_contrast.hpp"
#include "misc/map.hpp"

namespace op {
    bool ContrastOP::init(const map<string, string>& config) {
        get_value(config, "alpha", alpha_fno_, alpha_);
        get_value(config, "beta", beta_fno_, beta_);

        return is_init();
    }

    bool ContrastOP::is_init() {
        return true;
    }

    cv::Mat ContrastOP::execute_current(const cv::Mat& img,
                                        const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "ContrastOp is not initialized";
        } else {
            double alpha = get_field_value(fields, alpha_fno_, alpha_);
            double beta = get_field_value(fields, beta_fno_, beta_);
            img.convertTo(ret, -1, alpha, beta);
        }

        return ret;
    }
}
