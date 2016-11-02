#include "cmd/op_show.hpp"

namespace op {
    bool ShowOP::init(const map<string, string>& config) {
        return true;
    }

    bool ShowOP::is_init() {
        return true;
    }

    cv::Mat ShowOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        cout << get_key(fields) << " " << img.cols << " " << img.rows
             << " " << img.channels() << endl;
        cv::imshow("image", img);
        cv::waitKey();
        return img;
    }
}
