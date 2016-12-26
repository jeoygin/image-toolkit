#ifndef OP_SHARPEN
#define OP_SHARPEN

#include "operation.hpp"

namespace op {
    class SharpenOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "SharpenOP";
        }

    private:
        int sigmax_fno_ = 3;
        double sigmax_ = 3;
        int sigmay_fno_ = 4;
        double sigmay_ = 1;
        int threshold_fno_ = 5;
        double threshold_ = 5;
        int amount_fno_ = 6;
        double amount_ = 1;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_SHARPEN
