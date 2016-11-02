#ifndef OP_ROTATE
#define OP_ROTATE

#include "operation.hpp"

namespace op {
    class RotateOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "RotateOP";
        }

    private:
        int angle_fno_ = 3;
        int angle_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_ROTATE
