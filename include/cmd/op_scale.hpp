#ifndef OP_SCALE
#define OP_SCALE

#include "operation.hpp"

namespace op {
    class ScaleOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "ScaleOP";
        }

    private:
        int width_fno_ = 3;
        double width_ = 1;
        int height_fno_ = 4;
        double height_ = 1;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_SCALE
