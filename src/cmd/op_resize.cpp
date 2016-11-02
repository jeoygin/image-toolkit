#include "cmd/op_resize.hpp"
#include "misc/map.hpp"

namespace op {
    bool ResizeOP::init(const map<string, string>& config) {
        get_int_value(config, "width", width_fno_, width_);
        get_int_value(config, "height", height_fno_, height_);
        get_int_value(config, "keep", keep_fno_, keep_);

        return is_init();
    }

    bool ResizeOP::is_init() {
        return true;
    }

    cv::Mat ResizeOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "ResizeOp is not initialized";
        } else {
            int width = width_, height = height_, keep = keep_;

            if (width_fno_ > 0 && width_fno_ <= fields.size()) {
                width = std::stoi(fields[width_fno_ - 1]);
            }

            if (height_fno_ > 0 && height_fno_ <= fields.size()) {
                height = std::stoi(fields[height_fno_ - 1]);
            }

            if (width <= 0 || height <= 0) {
                LOG(ERROR) << "Invalid widthxheight ("
                           << width << "x" << height
                           << "): " << get_key(fields);
            } else {
                if (keep == 0) {
                    cv::resize(img, ret, cv::Size(width, height));
                } else {
                    int tmp_width, tmp_height;
                    if (width * img.rows < height * img.cols) {
                        tmp_width = width;
                        tmp_height = max(1, img.rows * width / img.cols);
                    } else {
                        tmp_height = height;
                        tmp_width = max(1, img.cols * height / img.rows);
                    }
                    cv::Mat tmp;
                    cv::resize(img, tmp, cv::Size(tmp_width, tmp_height));

                    ret = cv::Mat::zeros(width, height, img.type());
                    tmp.copyTo(ret(cv::Rect((width - tmp_width) / 2,
                                            (height - tmp_height) / 2,
                                            tmp_width, tmp_height)));
                }
            }
        }

        return ret;
    }
}
