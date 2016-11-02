#ifndef OP_BINARY
#define OP_BINARY

#include "operation.hpp"

namespace op {
    class BinaryOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "BinaryOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_BINARY
