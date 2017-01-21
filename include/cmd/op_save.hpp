#ifndef OP_SAVE
#define OP_SAVE

#include "operation.hpp"
#include "db/db.hpp"
#include "store/data_dumper.hpp"

namespace op {
    class SaveOP : public Operation {
    public:
        ~SaveOP() {}

        bool init(const std::map<string, string>& config);
        bool is_init();
        void flush();
        void finish();

        string name() {
            return "SaveOP";
        }

    private:
        shared_ptr<store::DataDumper> dumper_;
        int key_fno_ = 2;
        string key_;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_SAVE
