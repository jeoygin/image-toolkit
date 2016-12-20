#ifndef OPERATION
#define OPERATION

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.hpp>
#include <glog/logging.h>

#include "misc/map.hpp"

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

    template<class T>
    bool get_value(const map<string, string>& config, const string key,
                   int& fno, T& value) {
        string v = map_get(config, key);
        if (!v.empty()) {
            if (v.compare(0, 1, "$") == 0) {
                fno = std::stoi(v.substr(1));
            } else {
                fno = -1;
                std::stringstream ss(v);
                ss >> value;
            }
        }
        return true;
    }

    template<class T>
    T get_field_value(const vector<string>& fields, int fno, T default_value) {
        T value = default_value;
        if (fno > 0 && fno <= (int) fields.size()) {
            std::stringstream ss(fields[fno-1]);
            ss >> value;
        }
        return value;
    }
}

#endif // OPERATION
