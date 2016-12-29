#ifndef OP_UPDATE
#define OP_UPDATE

#include "operation.hpp"

namespace op {
    class UpdateOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "UpdateOP";
        }

    private:
        int from_fno_ = 3;
        int from_ = 0;
        int to_fno_ = 4;
        int to_ = 64;
        int value_fno_ = 5;
        int value_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_UPDATE
