#ifndef ENCODER_STRING_HPP
#define ENCODER_STRING_HPP

#include "encoder.hpp"

using namespace std;

namespace encode {
    class StringEncoder : public Encoder {
    public:
        StringEncoder() {}

        string name() {
            return "string";
        }

        string encode(const string& data) {
            return data;
        }

        bool decode(const string& encoded, string& decoded) {
            decoded = encoded;
            return true;
        }
    };
}

#endif // ENCODER_STRING_HPP
