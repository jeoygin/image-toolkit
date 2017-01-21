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

        string encode(const string& data) {
            return base64_encode(reinterpret_cast<const unsigned char*>(data.data()),
                                 data.size());
        }

        bool decode(const string& encoded, string& decoded) {
            if (!encoded.empty()) {
                return base64_decode(encoded, decoded);
            } else {
                return false;
            }
        }
    };
}

#endif // ENCODER_BASE64_HPP
