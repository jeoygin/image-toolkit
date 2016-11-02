#ifndef OP_TEMPLATE
#define OP_TEMPLATE

#include <map>
#include <boost/shared_ptr.hpp>

#include "operation.hpp"

namespace op {
    class TemplateOP : public Operation {
    public:
        TemplateOP() : fg_from_fno_(-1), fg_from_(-1),
                       fg_to_fno_(-1), fg_to_(-1) {}

        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "TemplateOP";
        }

    private:
        int template_fno_;
        string template_;
        int fg_from_fno_;
        int fg_from_;
        int fg_to_fno_;
        int fg_to_;
        std::map<std::string, boost::shared_ptr<cv::Mat>> template_cache_;

        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);

        boost::shared_ptr<cv::Mat> open_template(const string& template_file);
    };
}

#endif // OP_TEMPLATE
