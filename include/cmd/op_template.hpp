#ifndef OP_TEMPLATE
#define OP_TEMPLATE

#include <map>
#include <boost/shared_ptr.hpp>

#include "operation.hpp"

namespace op {
    class TemplateOP : public Operation {
    public:
        TemplateOP() {}

        bool init(const std::map<string, string>& config);
        bool is_init();

        string name() {
            return "TemplateOP";
        }

    private:
        int template_fno_;
        string template_;
        std::map<std::string, boost::shared_ptr<cv::Mat>> template_cache_;

        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);

        boost::shared_ptr<cv::Mat> open_template(const string& template_file);
    };
}

#endif // OP_TEMPLATE
