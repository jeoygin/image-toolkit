#ifndef OP_SAVE
#define OP_SAVE

#include "operation.hpp"
#include "db/db.hpp"

namespace op {
    class SaveOP : public Operation {
    public:
        ~SaveOP() {
            if (writer_) {
                delete writer_;
                writer_ = NULL;
            }

            if (db_) {
                db_->close();
                delete db_;
                db_ = NULL;
            }
        }

        bool init(const std::map<string, string>& config);
        bool is_init();
        void flush();
        void finish();

        string name() {
            return "SaveOP";
        }

    private:
        db::DB* db_;
        db::Writer* writer_;
        int key_fno_ = 2;
        string key_;
        cv::Mat execute_current(const cv::Mat& img,
                                const std::vector<std::string>& fields);
    };
}

#endif // OP_SAVE
