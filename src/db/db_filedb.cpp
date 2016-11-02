#include "db/db_filedb.hpp"
#include "fs/fs.hpp"

#include <boost/scoped_ptr.hpp>

namespace db {
    static bool read_file(const string& path, vector<unsigned char>& content) {
        ifstream ifs;
        ifs.open(path, ios::in|ios::binary|ios::ate);

        if (ifs.is_open()) {
            size_t size = ifs.tellg();
            ifs.seekg(0, ios::beg);
            content.resize(size);
            boost::scoped_ptr<char> data(new char[size]);
            ifs.read(reinterpret_cast<char*>(content.data()), size);
            return true;
        }

        return false;
    }

    static void write_file(const string&path, const char * content,
                           const unsigned int size) {
        string dir = fs::get_dirname(path);

        if (!dir.empty()) {
            fs::make_dir(dir);
        }

        ofstream ofs(path, ios::out|ios::binary);
        ofs.write(content, size);
        ofs.close();
    }

    FileDBIterator::FileDBIterator(const string& root) : root_(root) {
        files_.clear();
        fs::list_file(root_, files_);
        seek_to_first();
    }

    string FileDBIterator::value() {
        vector<unsigned char> bytes;
        this->value(bytes);
        if (!bytes.empty()) {
            return base64_encode(bytes.data(), bytes.size());
        }
        return "";
    }

    void FileDBIterator::value(vector<unsigned char>& value) {
        string path = root_ + "/" + *iter_;
        value.clear();
        read_file(path, value);
    }

    void FileDBWriter::put(const string& key, const vector<unsigned char>& value) {
        string path = root_ + "/" + key;
        write_file(path, reinterpret_cast<const char*>(value.data()),
                   value.size());
    }

    void FileDBWriter::put(const string& key, const string& value) {
        vector<unsigned char> decoded_value;
        base64_decode(value, decoded_value);
        put(key, decoded_value);
    }

    void FileDB::open(const string& source, Mode mode) {
        root_ = source;
    }

    string FileDB::get(const string& key) {
        vector<unsigned char> bytes;
        get(key, bytes);
        if (!bytes.empty()) {
            return base64_encode(bytes.data(), bytes.size());
        }
        return "";
    }

    void FileDB::get(const string& key, vector<unsigned char>& value) {
        string path = root_ + "/" + key;
        value.clear();
        read_file(path, value);
    }

    void FileDB::put(const string& key, const vector<unsigned char>& value) {
        string path = root_ + "/" + key;
        write_file(path, reinterpret_cast<const char*>(value.data()),
                   value.size());
    }

    void FileDB::put(const string& key, const string& value) {
        vector<unsigned char> decoded_value;
        base64_decode(value, decoded_value);
        put(key, decoded_value);
    }
}
