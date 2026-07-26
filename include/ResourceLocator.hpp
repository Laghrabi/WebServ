#ifndef RESOURCE_LOCATOR_HPP
# define RESOURCE_LOCATOR_HPP

# include "webserver.hpp"

/**
 * @enum ResourceType
 * @brief Represents the physical state of the requested resource on disk.
 */
enum ResourceType {
    RESOURCE_FILE,
    RESOURCE_DIRECTORY,
    RESOURCE_CGI,
    RESOURCE_NOT_FOUND,
    RESOURCE_FORBIDDEN
};

class ResourceLocator {
    public:
        ResourceLocator();
        ResourceLocator(const ResourceLocator& other);
        ResourceLocator& operator=(const ResourceLocator& other);
        ~ResourceLocator();

        std::string     resolvePath(const std::string& basePath, const RouteConfig* route) const;
        ResourceType    getResourceType(const std::string& physicalPath) const;
        bool            isCgiExtension(const std::string& physicalPath) const;


std::string buildPhysicalPath(const HttpRequest& request, std::string& base_path, std::string& resource) const ;
};

#endif
