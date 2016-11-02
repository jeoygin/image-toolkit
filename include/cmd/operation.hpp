#ifndef OPERATION
#define OPERATION

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.hpp>
#include <glog/logging.h>

using namespace std;

namespace op {
    class Operation {
    public:
        Operation() {}
        virtual ~Operation() {}
        virtual string name() = 0;
        virtual bool init(const std::map<string, string>& config) = 0;
        virtual bool is_init() = 0;
        virtual void flush() {}
        virtual void finish() {}

        void post(boost::shared_ptr<Operation> op) {
            post_operation_ = op;
        }

        cv::Mat execute(const cv::Mat& img,
                        const std::vector<std::string>& fields) {
            cv::Mat new_img = execute_current(img, fields);
            if (post_operation_ && new_img.rows > 0 && new_img.cols > 0) {
                return post_operation_.get()->execute(new_img, fields);
            } else {
                return new_img;
            }
        }
    private:
        boost::shared_ptr<Operation> post_operation_;

        virtual cv::Mat execute_current(const cv::Mat& img,
                                        const std::vector<std::string>& fields) = 0;

    protected:
        string get_key(const std::vector<std::string>& fields) {
            if (fields.size() > 0) {
                return fields[0];
            }
            return "";
        }
    };

    boost::shared_ptr<Operation> get_operation(const std::map<std::string,
                                               std::string>& config);

    bool create_ops_from_config(const string& opsfile,
                                vector<boost::shared_ptr<op::Operation>>& ops);

    bool get_int_value(const map<string, string>& config, const string key,
                       int& fno, int& value);

    bool get_string_value(const map<string, string>& config, const string key,
                          int& fno, string& value);
}

#endif // OPERATION
