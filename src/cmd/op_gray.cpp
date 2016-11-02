#include "cmd/op_gray.hpp"
#include "misc/map.hpp"

namespace op {
    bool GrayOP::init(const map<string, string>& config) {
        return is_init();
    }

    bool GrayOP::is_init() {
        return true;
    }

    cv::Mat GrayOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "GrayOp is not initialized";
        } else {
            if (img.channels() >= 3) {
                cv::cvtColor(img, ret, CV_BGR2GRAY);
            } else {
                ret = img;
            }
        }

        return ret;
    }
}
