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

        string encode(const vector<unsigned char>& data) {
            return string(data.begin(), data.end());
        }

        void decode(const string& text, vector<unsigned char>& data) {
            data.clear();
            std::copy(text.begin(), text.end(), std::back_inserter(data));
        }
    };
}

#endif // ENCODER_STRING_HPP
