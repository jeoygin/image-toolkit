#ifndef IMAGEDB_CONFIG
#define IMAGEDB_CONFIG

#include <iostream>
#include <string>

using namespace std;

class ImageDBConfig {
public:
    ImageDBConfig(const string& src, const string& dst,
                  const string& opsfile, const string listfile)
        : src_(src), dst_(dst), opsfile_(opsfile), listfile_(listfile) {}

    string src() {
        return src_;
    }

    string dst() {
        return dst_;
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
    string opsfile_;
    string listfile_;
};

#endif // IMAGEDB_CONFIG
