#ifndef COMMAND_HANDLER
#define COMMAND_HANDLER

#include <string>
#include <vector>
#include <queue>
#include <map>

#include "config.hpp"
#include "operation.hpp"
#include "store/data_loader.hpp"
#include "store/data_dumper.hpp"

namespace cmd {

    class CommandHandler {
    public:
        int process(const std::string& cmd, ImageDBConfig& config);
    };

    class CommandProcessor {
    public:
        virtual int run(boost::shared_ptr<store::DataLoader> loader,
                        const int idx, const std::vector<string>& fields) = 0;
        virtual void refresh() = 0;
        virtual void done() = 0;
        virtual bool good() = 0;
    };

    class PipeProcessor : public CommandProcessor {
    public:
        PipeProcessor(const std::vector<boost::shared_ptr<op::Operation>>& ops): ops_(ops) {}

        ~PipeProcessor() {

        }

        int run(boost::shared_ptr<store::DataLoader> loader, const int idx,
                const std::vector<string>& fields) {
            if (ops_.empty()) {
                return -1;
            } else if (!loader) {
                return -2;
            } else if (fields.size() < 1) {
                return -3;
            }

            string key = fields[0];
            auto it = cache_.find(key);
            cv::Mat img;
            if (it != cache_.end()) {
                img = *(it->second);
            } else {
                string img_str = loader->get(key);
                if (!img_str.empty()) {
                    vector<unsigned char> content(img_str.begin(), img_str.end());
                    boost::shared_ptr<cv::Mat> img_ptr(new cv::Mat());
                    *img_ptr = img = cv::imdecode(content, CV_LOAD_IMAGE_UNCHANGED);
                    cache_keys_.push(key);
                    cache_[key] = img_ptr;

                    while (cache_keys_.size() > 10000) {
                        std::string to_remove_key = cache_keys_.front();
                        cache_.erase(to_remove_key);
                        cache_keys_.pop();
                    }
                } else {
                    return -4;
                }
            }

            if (img.data != NULL) {
                ops_[0]->execute(img, fields);
            } else {
                return -4;
            }

            return 0;
        }

        void refresh() {
            for (int i = 0; i < (int) ops_.size(); i++) {
                ops_[i]->flush();
            }
        }

        void done() {
            for (int i = 0; i < (int) ops_.size(); i++) {
                ops_[i]->finish();
            }
        }

        bool good() {
            return ops_.size() > 0;
        }
    private:
        const std::vector<boost::shared_ptr<op::Operation>>& ops_;
        std::queue<std::string> cache_keys_;
        std::map<std::string, boost::shared_ptr<cv::Mat>> cache_;
    };

    class SaveProcessor : public CommandProcessor {
    public:
        SaveProcessor(const string& url,
                      boost::shared_ptr<encode::Encoder> encoder) {
            boost::shared_ptr<db::DB> db = db::open_db(url, db::WRITE);
            if (db) {
                dumper_.reset(new store::DataDumper(db, encoder));
            }
        }

        ~SaveProcessor() {}

        int run(boost::shared_ptr<store::DataLoader> loader, const int idx,
                const std::vector<string>& fields) {
            if (!dumper_) {
                return -1;
            } else if (!loader) {
                return -2;
            } else if (fields.size() < 2) {
                return -3;
            }

            std::string src_key = fields[0], dst_key = fields[1];
            string content = loader->get(src_key);
            if (content.empty()) {
                return -4;
            }
            dumper_->put(dst_key, content);

            return 0;
        }

        void refresh() {
            if (dumper_) {
                dumper_->flush();
            }
        }

        void done() {}

        bool good() {
            return dumper_ != NULL;
        }
    private:
        boost::shared_ptr<store::DataDumper> dumper_;
    };

    class DeleteProcessor : public CommandProcessor {
    public:
        DeleteProcessor(const string& url, const string& save_url,
                        boost::shared_ptr<encode::Encoder> encoder) {
            if (!save_url.empty()) {
                save_ = true;
                boost::shared_ptr<db::DB> save_db = db::open_db(save_url, 
                                                                db::WRITE);
                if (save_db) {
                    save_dumper_.reset(new store::DataDumper(save_db, encoder));
                }
            }

            boost::shared_ptr<db::DB> db = db::open_db(url, db::WRITE);
            if (db) {
                dumper_.reset(new store::DataDumper(db, encoder));
                loader_.reset(new store::DataLoader(db, encoder));
            }
        }

        ~DeleteProcessor() {}

        int run(boost::shared_ptr<store::DataLoader> loader, const int idx,
                const std::vector<string>& fields) {
            if (!dumper_ || (save_ && !loader_ && !save_dumper_)) {
                return -1;
            } else if (!loader) {
                return -2;
            } else if ((!save_ && fields.size() < 1)
                       || (save_ && fields.size() < 2)) {
                return -3;
            }

            std::string key = fields[0];
            if (save_) {
                std::string dst_key = fields[1];
                string content = loader_->get(key);
                if (content.empty()) {
                    return -4;
                }
                save_dumper_->put(dst_key, content);
            }

            if (!dumper_->del(key)) {
                return -4;
            }

            return 0;
        }

        void refresh() {
            if (dumper_) {
                dumper_->flush();
            }

            if (save_dumper_) {
                save_dumper_->flush();
            }
        }

        void done() {
        }

        bool good() {
            return dumper_ != NULL;
        }
    private:
        boost::shared_ptr<store::DataLoader> loader_;
        boost::shared_ptr<store::DataDumper> dumper_;
        boost::shared_ptr<store::DataDumper> save_dumper_;
        bool save_ = false;
    };
} // cmd

#endif
