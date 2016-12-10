#ifndef OP_LABEL
#define OP_LABEL

#include "operation.hpp"

namespace op {
    class LabelOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "LabelOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_LABEL
