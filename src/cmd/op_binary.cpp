#include "cmd/op_binary.hpp"
#include "misc/map.hpp"

namespace op {
    bool BinaryOP::init(const map<string, string>& config) {
        return is_init();
    }

    bool BinaryOP::is_init() {
        return true;
    }

    cv::Mat BinaryOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "BinaryOp is not initialized";
        } else {
            cv::Mat gray;
            if (img.channels() >= 3) {
                cv::cvtColor(img, gray, CV_BGR2GRAY);
            } else {
                gray = img.clone();
            }
            cv::threshold(gray, ret, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);
        }

        return ret;
    }
}
