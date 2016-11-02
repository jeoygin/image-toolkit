#ifndef FS_HPP
#define FS_HPP

#include <unistd.h>

#include <string>
#include <vector>

using namespace std;

namespace fs {
    bool dir_exists(const char* dir);
    bool dir_exists(const string& dir);
    bool file_exists(const char* file);
    bool file_exists(const string& file);
    int make_dir(const char * dir, mode_t mode);
    int make_dir(const string& dir);
    string get_dirname(const string& path);
    string get_basename(const string& path);
    void list_file(const string& dir, vector<string>& files);
} // namespace fs

#endif // FS_HPP
