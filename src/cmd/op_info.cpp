#include "cmd/op_info.hpp"

namespace op {
    bool InfoOP::init(const map<string, string>& config) {
        return true;
    }

    bool InfoOP::is_init() {
        return true;
    }

    cv::Mat InfoOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        cout << get_key(fields) << " " << img.cols << " " << img.rows
             << " " << img.channels() << endl;
        return img;
    }
}
