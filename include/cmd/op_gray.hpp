#ifndef OP_GRAY
#define OP_GRAY

#include "operation.hpp"

namespace op {
    class GrayOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "GrayOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_GRAY
