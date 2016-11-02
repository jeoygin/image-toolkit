#ifndef OP_CAT
#define OP_CAT

#include "operation.hpp"

namespace op {
    class CatOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "CatOP";
        }

    private:
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_CAT
