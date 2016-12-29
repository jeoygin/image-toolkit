#include "cmd/op_update.hpp"
#include "misc/map.hpp"

namespace op {
    bool UpdateOP::init(const map<string, string>& config) {
        get_value(config, "from", from_fno_, from_);
        get_value(config, "to", to_fno_, to_);
        get_value(config, "value", value_fno_, value_);

        return is_init();
    }

    bool UpdateOP::is_init() {
        return true;
    }

    cv::Mat UpdateOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "UpdateOp is not initialized";
        } else {
            int from = get_field_value(fields, from_fno_, from_);
            int to = get_field_value(fields, to_fno_, to_);
            int value = min(get_field_value(fields, value_fno_, value_), 255);

            if (value < 0) {
                int sum = 0, cnt = 0;
                for (int r = 0; r < img.rows; r++) {
                    for (int c = 0; c < img.cols; c++) {
                        uchar g = img.at<uchar>(r, c);
                        if (g >= from && g <= to) {
                            sum += g;
                            cnt++;
                        }
                    }
                }

                if (cnt > 0) {
                    value = sum / cnt;
                } else {
                    value = 0;
                }
            }

            ret = img.clone();
            for (int r = 0; r < ret.rows; r++) {
                for (int c = 0; c < ret.cols; c++) {
                    uchar& g = ret.at<uchar>(r, c);
                    if (g >= from && g <= to) {
                        g = value;
                    }
                }
            }
        }

        return ret;
    }
}
