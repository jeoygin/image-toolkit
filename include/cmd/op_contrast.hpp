#ifndef OP_CONTRAST
#define OP_CONTRAST

#include "operation.hpp"

namespace op {
    class ContrastOP : public Operation {
    public:
        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "ContrastOP";
        }

    private:
        int alpha_fno_ = 3;
        double alpha_ = 1;
        int beta_fno_ = 4;
        double beta_ = 0;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_CONTRAST
