#include "db/db_filedb.hpp"
#include "fs/fs.hpp"

#include <boost/scoped_ptr.hpp>

namespace db {
    static bool read_file(const string& path, vector<char>& content) {
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

    static bool remove_file(const string& path) {
        return fs::rm(path);
    }

    FileDBIterator::FileDBIterator(const string& root)
        : root_(root), inited_(false), random_(false) {
    }

    string FileDBIterator::value() {
        string path = root_ + "/" + key();
        vector<char> content;
        read_file(path, content);
        return string(content.begin(), content.end());
    } 

    bool FileDBIterator::valid() {
        if (random_) {
            return fs::file_exists(root_ + "/" + key_);
        }
        if (!inited_) {
            init();
        }
        return iter_ < keys_.end();
    }

    void FileDBIterator::init() {
        if (!inited_) {
            keys_.clear();
            key_map_.clear();
            fs::list_file(root_, keys_);
            for (size_t i = 0; i < keys_.size(); i++) {
                key_map_[keys_[i]] = i;
            }
            inited_ = true;
            seek_to_first();
        }
    }

    void FileDBWriter::put(const string& key, const string& value) {
        string path = root_ + "/" + key;
        write_file(path, value.data(), value.size());
    }

    bool FileDBWriter::del(const string& key) {
        string path = root_ + "/" + key;
        return remove_file(path);
    }
}
