#ifndef OP_INFO
#define OP_INFO

#include "operation.hpp"

namespace op {
    class InfoOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "InfoOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_INFO
