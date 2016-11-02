#include "cmd/op_sizeup.hpp"
#include "misc/map.hpp"

namespace op {
    bool SizeupOP::init(const map<string, string>& config) {
        get_int_value(config, "size", size_fno_, size_);

        return is_init();
    }

    bool SizeupOP::is_init() {
        return size_ > 0;
    }

    cv::Mat SizeupOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "SizeupOp is not initialized";
        } else {
            int size = size_;

            if (size_fno_ > 0 && size_fno_ <= fields.size()) {
                size = std::stoi(fields[size_fno_ - 1]);
            }

            if (size <= 0) {
                LOG(ERROR) << "Invalid size (" << size <<"): " << get_key(fields);
            } else {
                double scale = 1.0 * size / max(img.rows, img.cols);
                cv::resize(img, ret,
                           cv::Size((int)(img.cols * scale + 0.5),
                                    (int)(img.rows * scale + 0.5)),
                           CV_INTER_CUBIC);
            }
        }

        return ret;
    }
}
