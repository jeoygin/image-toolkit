#ifndef OP_SIZEUP
#define OP_SIZEUP

#include "operation.hpp"

namespace op {
    class SizeupOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "SizeupOP";
        }

    private:
        int size_fno_ = 3;
        int size_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_SIZEUP
