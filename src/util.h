#include <sstream>
#include <string>
#include <vector>

namespace exacto {

namespace util {

void split(const std::string& s, char delim, std::vector<std::string>* output) {
  std::stringstream as_stream(s);
  std::string next;
  while (std::getline(as_stream, next, delim)) {
    output->push_back(next);
  }
}

}  // namespace util

}  // namespace exacto
