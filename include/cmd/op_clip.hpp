#ifndef OP_CLIP
#define OP_CLIP

#include "operation.hpp"

namespace op {
    class ClipOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "ClipOP";
        }

    private:
        int x_fno_ = 3;
        int x_ = 0;
        int y_fno_ = 4;
        int y_ = 0;
        int w_fno_ = 5;
        int w_ = 0;
        int h_fno_ = 6;
        int h_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_CLIP
