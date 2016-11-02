#include <string.h>

#include "cmd/op_cat.hpp"
#include "base64/base64.h"

namespace op {
    bool CatOP::init(const map<string, string>& config) {
        return true;
    }

    bool CatOP::is_init() {
        return true;
    }

    cv::Mat CatOP::execute_current(const cv::Mat& img,
                                    const vector<string>& fields) {
        string key = get_key(fields);
        cout << key << " " << img.cols << " " << img.rows
             << " " << img.channels() << endl;

        std::size_t last_dot = key.rfind(".");
        string ext;
        if (last_dot != string::npos) {
            ext = key.substr(last_dot);
        }

        vector<unsigned char> img_content;
        cv::imencode(ext, img, img_content);
        string base64_string = base64_encode(img_content.data(), img_content.size());

        char const* term = std::getenv("TERM");
        char const* tmux = std::getenv("TMUX");
        bool in_tmux = false;
        if (term != NULL && tmux != NULL) {
            if (strncmp(term, "xterm", 5) == 0
                || strncmp(term, "screen", 6) == 0) {
                if (tmux[0] > 0) {
                    in_tmux = true;
                }
            }
        }

        if (in_tmux) {
            std::cout << "\033Ptmux;\033\033]";
        } else {
            std::cout << "\033]";
        }

        const unsigned char * key_ptr =
            reinterpret_cast<const unsigned char*>(key.data());
        std::cout << "1337;File=name="
                  << base64_encode(key_ptr, key.length())
                  << ";size=" << img_content.size()
                  << ";inline=1:" << base64_string;

        if (in_tmux) {
            std::cout << "\a\033\\";
        } else {
            std::cout << "\a";
        }

        std::cout << std::endl;

        return img;
    }
}
