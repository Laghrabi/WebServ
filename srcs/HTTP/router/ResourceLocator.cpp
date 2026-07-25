#include "../../../include/webserver.hpp"

ResourceLocator::ResourceLocator() {}

ResourceLocator::ResourceLocator(const ResourceLocator& other) {
    *this = other;
}

ResourceLocator& ResourceLocator::operator=(const ResourceLocator& other) {
    (void)other;
    return (*this);
}

ResourceLocator::~ResourceLocator() {}

/**
 * @brief Constructs the absolute physical file path from a request URI.
 * * Resolves the root directory by prioritizing the route's `root` directive, 
 * falling back to the server `root`, or defaulting to the current directory.
 * * Ensures proper concatenation of the root and URI segments.
 */
std::string ResourceLocator::buildPhysicalPath(const std::string& uri, const RouteConfig* route, const Server* server) const {
   std::string rootPath = "";
    
    if (route)
        rootPath = route->getRoot();
    if (rootPath.empty() && server)
        rootPath = server->getRoot();
		// here we have to options
		// option number 2(directely) if no match and server has root automatically
		// match and root server will be added to the requested uri
    // if (rootPath.empty())
    //     rootPath = ".";
    
    bool rootEndsWithSlash = (rootPath[rootPath.length() - 1] == '/');
    bool uriStartsWithSlash = (!uri.empty() && uri[0] == '/');

    if (rootEndsWithSlash && uriStartsWithSlash)
        return (rootPath + uri.substr(1));
    else if (!rootEndsWithSlash && !uriStartsWithSlash)
        return (rootPath + "/" + uri);
    
    return (rootPath + uri);
}

/**
 * @brief Determines the resource type using stat() system calls.
 * * Maps file system attributes to internal resource types, identifying 
 * directories, files, or access-restricted paths.
 * @return RESOURCE_DIRECTORY, RESOURCE_FILE, RESOURCE_FORBIDDEN, or RESOURCE_NOT_FOUND.
 */
ResourceType ResourceLocator::getResourceType(const std::string& physicalPath) const {
    struct stat fileInfo;

    if (stat(physicalPath.c_str(), &fileInfo) != 0) {
#ifdef DEBUG
			std::cout << "resource not found: " << physicalPath.c_str() << "\n";
#endif

        if (errno == EACCES)
				{
#ifdef DEBUG
					std::cout << "[(ROUTING) this resource is forbidden]\n" << "\n";
#endif
            return (RESOURCE_FORBIDDEN);
				}
        return (RESOURCE_NOT_FOUND);
    }

    if (S_ISDIR(fileInfo.st_mode))
        return (RESOURCE_DIRECTORY);
    if (S_ISREG(fileInfo.st_mode))
        return (RESOURCE_FILE);
    
#ifdef DEBUG
			std::cout << "forbidden: " << physicalPath.c_str() << "\n";
#endif
    return (RESOURCE_FORBIDDEN);
}

/**
 * @brief Resolves a URI to a concrete physical path, including directory index files.
 * * If the resolved path is a directory, it iterates through the configured 
 * index files (e.g., "index.html") to find an existing file match.
 */
std::string ResourceLocator::resolvePath(const std::string& uri, const RouteConfig* route, const Server* server) const {
    std::string basePath = buildPhysicalPath(uri, route, server);

    // std::cout << "BASE_PATH= " << basePath << std::endl;
    if (route && getResourceType(basePath) == RESOURCE_DIRECTORY) {
        const std::list<std::string>& indexFiles = route->getIndexes();

        for (std::list<std::string>::const_iterator it = indexFiles.begin(); it != indexFiles.end(); ++it) {
            std::string indexPath = basePath;

            if (!basePath.empty() && basePath[basePath.length() - 1] != '/')
                indexPath += "/";
            indexPath += *it;

            if (getResourceType(indexPath) == RESOURCE_FILE)
                return (indexPath);
        }
    }
    return (basePath);
}
