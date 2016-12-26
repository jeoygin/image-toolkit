#include "cmd/op_sharpen.hpp"
#include "misc/map.hpp"

namespace op {
    bool SharpenOP::init(const map<string, string>& config) {
        get_value(config, "sigmax", sigmax_fno_, sigmax_);
        get_value(config, "sigmay", sigmay_fno_, sigmay_);
        get_value(config, "threshold", threshold_fno_, threshold_);
        get_value(config, "amount", amount_fno_, amount_);

        return is_init();
    }

    bool SharpenOP::is_init() {
        return true;
    }

    cv::Mat SharpenOP::execute_current(const cv::Mat& img,
                                       const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "SharpenOp is not initialized";
        } else {
            double sigmax = get_field_value(fields, sigmax_fno_, sigmax_);
            double sigmay = get_field_value(fields, sigmay_fno_, sigmay_);
            double threshold = get_field_value(fields, threshold_fno_, threshold_);
            double amount = get_field_value(fields, amount_fno_, amount_);

            cv::Mat blurred;
            cv::GaussianBlur(img, blurred, cv::Size(), sigmax, sigmay);
            cv::Mat lowContrastMask = abs(img - blurred) < threshold;
            ret = img * (1 + amount) + blurred * (-amount);
            img.copyTo(ret, lowContrastMask);
        }

        return ret;
    }
}
