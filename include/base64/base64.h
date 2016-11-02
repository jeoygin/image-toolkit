#include <string>
#include <vector>

 std::string base64_encode(const unsigned char * bytes_to_encode, unsigned int len);
 bool base64_decode(const std::string& s, std::vector<unsigned char>& decoded);
