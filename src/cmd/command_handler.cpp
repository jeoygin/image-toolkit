#include "cmd/command_handler.hpp"
#include "db/db.hpp"
#include "base64/base64.h"

#include <map>
#include <iostream>
#include <fstream>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <yaml-cpp/yaml.h>

namespace cmd {
    static int process_list(ImageDBConfig& config, std::istream& list_stream,
                            boost::shared_ptr<CommandProcessor> processor) {
        db::DB* src_db = db::open_db(config.src(), db::READ);
        if (!src_db) {
            LOG(ERROR) << "Failed to open db: " << config.src();
            return -1;
        }

        int processed = 0;
        vector<string> fields;
        vector<unsigned char> img_content;
        for (string line; std::getline(list_stream, line); ) {
            if (line.empty()) {
                continue;
            }

            fields.clear();
            size_t pos, cur_pos = 0;
            while ((pos = line.find("::", cur_pos)) != string::npos) {
                fields.push_back(line.substr(cur_pos, pos - cur_pos));
                cur_pos = pos + 2;
            }
            if (cur_pos < line.size()) {
                fields.push_back(line.substr(cur_pos));
            }

            try {
                string key = fields[0];
                int ret_code = processor->run(src_db, processed, fields);
                if (ret_code == -1) {
                    LOG(ERROR) << "Invalid configuration: " << key << endl;
                } else if (ret_code == -2) {
                    LOG(ERROR) << "Invalid db: " << key << endl;
                } else if (ret_code == -3) {
                    LOG(ERROR) << "Invalid fields: " << key << endl;
                } else if (ret_code == -4) {
                    LOG(ERROR) << "No such image " << key << endl;
                }
            } catch(cv::Exception& e) {
                LOG(ERROR) << "FAIL: " << line << endl << e.what() << endl;
            }

            ++processed;
            if (processed % 1000 == 0) {
                LOG(INFO) << "Processed " << processed << " records." << endl;
                processor->refresh();
            }
        }

        if (processed % 1000 != 0) {
            LOG(INFO) << "Processed " << processed << " records." << endl;
            processor->refresh();
        }

        processor->done();

        return 0;
    }

    int CommandHandler::process(const string& cmd, ImageDBConfig& config) {
        if (cmd == "list") {
            db::DB* db = db::open_db(config.src(), db::READ);
            if (!db) {
                LOG(ERROR) << "Failed to open db: " << config.src();
                return -1;
            }
            for (db::Iterator* it = db->new_iterator(); it->valid(); it->next()) {
                cout << it->key() << endl;
            }
            return 0;
        }

        std::istream list_stream(cin.rdbuf());
        std::ifstream list_file_stream;
        if (!config.listfile().empty()) {
            list_file_stream.open(config.listfile());
            if (list_file_stream.is_open()) {
                list_stream.rdbuf(list_file_stream.rdbuf());
            } else {
                LOG(ERROR) << "Failed to open listfile: " << config.listfile() << endl;
                return -1;
            }
        }

        int ret_code = 0;
        boost::shared_ptr<CommandProcessor> processor;
        std::vector<boost::shared_ptr<op::Operation>> ops;
        if (cmd == "pipe") {
            if (!op::create_ops_from_config(config.opsfile(), ops)) {
                return -1;
            }
            processor.reset(new PipeProcessor(ops));
        } else if (cmd == "save") {
            processor.reset(new SaveProcessor(config.dst()));
        } else {
            std::map<string, string> ops_config;
            ops_config["cmd"] = cmd;
            if (!config.dst().empty()) {
                ops_config["db"] = config.dst();
            }

            boost::shared_ptr<op::Operation> op_ptr = op::get_operation(ops_config);
            if (op_ptr) {
                ops.push_back(op_ptr);
            } else {
                LOG(ERROR) << "Unsupported command: " << cmd;
                return -1;
            }

            if (!config.dst().empty()) {
                ops_config["cmd"] = "save";
                ops_config["key"] = "$2";
                boost::shared_ptr<op::Operation> save_op = op::get_operation(ops_config);
                ops.push_back(save_op);
                op_ptr.get()->post(save_op);
            }

            processor.reset(new PipeProcessor(ops));
        }

        if (processor->good()) {
            ret_code = process_list(config, list_stream, processor);
        } else {
            ret_code = -1;
        }

        if (list_file_stream.is_open()) {
            list_file_stream.close();
        }

        return ret_code;
    }
}
