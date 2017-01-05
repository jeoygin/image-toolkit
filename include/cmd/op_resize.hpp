#ifndef OP_RESIZE
#define OP_RESIZE

#include "operation.hpp"

namespace op {
    class ResizeOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "ResizeOP";
        }

    private:
        int width_fno_ = 3;
        int width_ = 0;
        int height_fno_ = 4;
        int height_ = 0;
        int keep_fno_ = 5;
        int keep_ = 0;
        int bg_fno_ = 6;
        int bg_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);

        cv::Mat generate_background(const cv::Mat& img,
                                    int width, int height, int bg);
    };
}

#endif // OP_RESIZE
