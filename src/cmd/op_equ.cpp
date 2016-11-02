#include "cmd/op_equ.hpp"
#include "misc/map.hpp"

namespace op {
    bool EquOP::init(const map<string, string>& config) {
        return is_init();
    }

    bool EquOP::is_init() {
        return true;
    }

    cv::Mat EquOP::execute_current(const cv::Mat& img,
                                   const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "EquOp is not initialized";
        } else {
            int min = 255, max = 0, val;

            std::vector<int> vec;
            for (int x = 0; x < img.cols; x++) {
                for (int y = 0; y < img.rows; y++) {
                    val = img.ptr<uchar>(y)[x];
                    vec.push_back(val);
                }
            }
            sort(vec.begin(), vec.end());

            min = vec[vec.size() * 0.05];
            max = vec[vec.size() * 0.95];
            ret = img.clone();
            for (int x = 0; x < img.cols; x++) {
                for (int y = 0; y < img.rows; y++) {
                    val = img.ptr<uchar>(y)[x];
                    val = val < min ? min : val > max ? max : val;
                    int rval = (int)(((val - min) / (float)(max - min + 1)) * 255);
                    ret.ptr<uchar>(y)[x] = rval;
                }
            }
        }

        return ret;
    }
}
