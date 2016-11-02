#ifndef OP_INVERT
#define OP_INVERT

#include "operation.hpp"

namespace op {
    class InvertOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "InvertOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_INVERT
