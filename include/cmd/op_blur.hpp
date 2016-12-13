#ifndef OP_BLUR
#define OP_BLUR

#include "operation.hpp"

namespace op {
    class BlurOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "BlurOP";
        }

    private:
        int kernel_fno_ = 3;
        int kernel_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_BLUR
