#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <glog/logging.h>

using namespace std;

namespace encode {
    class Encoder {
    public:
        Encoder() {}
        virtual ~Encoder() {}
        virtual string name() = 0;
        virtual string encode(const vector<unsigned char>& data) = 0;
        virtual void decode(const string& text, vector<unsigned char>& data) = 0;
    };

    boost::shared_ptr<Encoder> get_encoder(const std::string& name);
}

#endif // ENCODER_HPP
