#ifndef ENCODER_BASE64_HPP
#define ENCODER_BASE64_HPP

#include "base64/base64.h"
#include "encoder.hpp"

using namespace std;

namespace encode {
    class Base64Encoder : public Encoder {
    public:
        Base64Encoder() {}

        string name() {
            return "base64";
        }

        string encode(const vector<unsigned char>& data) {
            return base64_encode(data.data(), data.size());
        }

        void decode(const string& text, vector<unsigned char>& data) {
            data.clear();
            if (!text.empty()) {
                base64_decode(text, data);
            }
        }
    };
}

#endif // ENCODER_BASE64_HPP
