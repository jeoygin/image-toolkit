#include "cmd/op_label.hpp"

namespace op {
    bool LabelOP::init(const map<string, string>& config) {
        return true;
    }

    bool LabelOP::is_init() {
        return true;
    }

    cv::Mat LabelOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        cout << get_key(fields);
        for (size_t i = 1; i < fields.size(); i++) {
            cout << " " << fields[i];
        }
        cv::imshow("image", img);
        int pressed_key = cv::waitKey();
        if (pressed_key >= '0' && pressed_key <= '9') {
            cout << " " << (char) pressed_key;
        }
        cout << endl;
        return img;
    }
}
