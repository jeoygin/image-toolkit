#ifndef OP_SHOW
#define OP_SHOW

#include "operation.hpp"

namespace op {
    class ShowOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "ShowOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_SHOW
