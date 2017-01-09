#ifndef IMAGEDB_CONFIG
#define IMAGEDB_CONFIG

#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>

#include "encode/encoder.hpp"

using namespace std;

class ImageDBConfig {
public:
    ImageDBConfig(const string& src, const string& dst,
                  const boost::shared_ptr<encode::Encoder>& src_encoder,
                  const boost::shared_ptr<encode::Encoder>& dst_encoder,
                  const string& opsfile, const string listfile)
        : src_(src), dst_(dst),
          src_encoder_(src_encoder), dst_encoder_(dst_encoder),
          opsfile_(opsfile), listfile_(listfile) {}

    string src() {
        return src_;
    }

    string dst() {
        return dst_;
    }

    boost::shared_ptr<encode::Encoder> src_encoder() {
        return src_encoder_;
    }

    boost::shared_ptr<encode::Encoder> dst_encoder() {
        return dst_encoder_;
    }

    string opsfile() {
        return opsfile_;
    }

    string listfile() {
        return listfile_;
    }
private:
    string src_;
    string dst_;
    boost::shared_ptr<encode::Encoder> src_encoder_;
    boost::shared_ptr<encode::Encoder> dst_encoder_;
    string opsfile_;
    string listfile_;
};

#endif // IMAGEDB_CONFIG
