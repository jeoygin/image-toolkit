#include "cmd/op_invert.hpp"
#include "misc/map.hpp"

namespace op {
    bool InvertOP::init(const map<string, string>& config) {
        return is_init();
    }

    bool InvertOP::is_init() {
        return true;
    }

    cv::Mat InvertOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "InvertOp is not initialized";
        } else {
            ret = 255 - img;
        }

        return ret;
    }
}
