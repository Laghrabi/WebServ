#include "../../include/webserver.hpp"

std::string joinPaths(const std::string& path1, const std::string& path2) {
    if (path1.empty() || path2.empty())
        return (path1);

    bool p1EndsWithSlash = (path1[path1.length() - 1] == '/');
    bool p2StartsWithSlash = (path2[0] == '/');

    if (p1EndsWithSlash && p2StartsWithSlash) {
        return (path1 + path2.substr(1));
    } 
    else if (!p1EndsWithSlash && !p2StartsWithSlash) {
        return (path1 + "/" + path2);
    } 
    else {
        return (path1 + path2);
    }
}