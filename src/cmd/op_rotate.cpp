#include "cmd/op_rotate.hpp"
#include "misc/map.hpp"

namespace op {
    bool RotateOP::init(const map<string, string>& config) {
        get_int_value(config, "angle", angle_fno_, angle_);

        return is_init();
    }

    bool RotateOP::is_init() {
        return true;
    }

    cv::Mat RotateOP::execute_current(const cv::Mat& img,
                                      const vector<string>& fields) {
        cv::Mat ret;
        if (!is_init()) {
            LOG(ERROR) << "RotateOp is not initialized";
        } else {
            int angle = angle_;

            if (angle_fno_ > 0 && angle_fno_ <= fields.size()) {
                angle = std::stoi(fields[angle_fno_ - 1]);
            }

            cv::Point2f center(img.cols/2.0F, img.rows/2.0F);
            cv::Mat rot = getRotationMatrix2D(center, angle, 1.0);
            cv::Rect bbox = cv::RotatedRect(center, img.size(), angle).boundingRect();
            rot.at<double>(0, 2) += bbox.width/2.0 - center.x;
            rot.at<double>(1, 2) += bbox.height/2.0 - center.y;
            warpAffine(img, ret, rot, bbox.size(), cv::INTER_CUBIC);
        }

        return ret;
    }
}
