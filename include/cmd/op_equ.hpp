#ifndef OP_EQU
#define OP_EQU

#include "operation.hpp"

namespace op {
    class EquOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "EquOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_EQU
