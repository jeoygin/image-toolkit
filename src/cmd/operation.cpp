#include "cmd/operation.hpp"
#include "cmd/op_binary.hpp"
#include "cmd/op_cat.hpp"
#include "cmd/op_clip.hpp"
#include "cmd/op_equ.hpp"
#include "cmd/op_gray.hpp"
#include "cmd/op_invert.hpp"
#include "cmd/op_info.hpp"
#include "cmd/op_resize.hpp"
#include "cmd/op_rotate.hpp"
#include "cmd/op_save.hpp"
#include "cmd/op_show.hpp"
#include "cmd/op_sizeup.hpp"
#include "cmd/op_template.hpp"
#include "misc/map.hpp"

#include <glog/logging.h>
#include <yaml-cpp/yaml.h>

namespace op {
    boost::shared_ptr<Operation> get_operation(const std::map<std::string, std::string>& config) {
        std::string cmd = map_get(config, "cmd");
        boost::shared_ptr<Operation> op_ptr;
        Operation* op = NULL;
        if (cmd == "binary") {
            op = new BinaryOP();
        } else if (cmd == "cat") {
            op = new CatOP();
        } else if (cmd == "clip") {
            op = new ClipOP();
        } else if (cmd == "equ") {
            op = new EquOP();
        } else if (cmd == "gray") {
            op = new GrayOP();
        } else if (cmd == "info") {
            op = new InfoOP();
        } else if (cmd == "invert") {
            op = new InvertOP();
        } else if (cmd == "resize") {
            op = new ResizeOP();
        } else if (cmd == "rotate") {
            op = new RotateOP();
        } else if (cmd == "save") {
            op = new SaveOP();
        } else if (cmd == "show") {
            op = new ShowOP();
        } else if (cmd == "sizeup") {
            op = new SizeupOP();
        } else if (cmd == "template") {
            op = new TemplateOP();
        }

        if (op != NULL) {
            op_ptr.reset(op);
            op->init(config);
        }

        return op_ptr;
    }

    bool create_ops_from_config(const string& opsfile,
                                vector<boost::shared_ptr<op::Operation>>& ops) {
        ops.clear();
        if (!opsfile.empty()) {
            YAML::Node ops_root = YAML::LoadFile(opsfile);
            if (!ops_root.IsSequence()) {
                LOG(ERROR) << "Error opsfile format: " << opsfile;
                return false;
            }

            map<string, string> ops_config;

            for (std::size_t i = 0; i < ops_root.size(); i++) {
                if (!ops_root[i].IsMap()) {
                    LOG(ERROR) << "Error opsfile format: " << opsfile;
                    return false;
                }

                ops_config.clear();

                for (YAML::const_iterator it=ops_root[i].begin();
                     it != ops_root[i].end(); ++it) {
                    ops_config[it->first.as<std::string>()]
                        = it->second.as<std::string>();
                }

                boost::shared_ptr<op::Operation> op_ptr = op::get_operation(ops_config);
                if (op_ptr) {
                    ops.push_back(op_ptr);
                } else {
                    LOG(ERROR) << "Invalid opsfile command (" << i + 1 << ")";
                    return false;
                }
            }
        }

        for (std::size_t i = 1; i < ops.size(); i++) {
            ops[i-1].get()->post(ops[i]);
        }

        return true;
    }

    bool get_int_value(const map<string, string>& config, const string key,
                       int& fno, int& value) {
        string v = map_get(config, key);
        if (!v.empty()) {
            if (v.compare(0, 1, "$") == 0) {
                fno = std::stoi(v.substr(1));
            } else {
                fno = -1;
                value = std::stoi(v);
            }
        }
        return true;
    }

    bool get_string_value(const map<string, string>& config, const string key,
                          int& fno, string& value) {
        string v = map_get(config, key);
        if (!v.empty()) {
            if (v.compare(0, 1, "$") == 0) {
                fno = std::stoi(v.substr(1));
            } else {
                fno = -1;
                value = v;
            }
        }
        return true;
    }
}
