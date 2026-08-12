#include "ResourceLocator.hpp"

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

std::string ResourceLocator::buildPhysicalPath(const HttpRequest& request, std::string& base_path, std::string& resource) const {
	std::string rootPath = "";

	const RouteConfig* route = request._routeResult.route;
	std::string uri = request.getRouteUri();

	const Location* test = dynamic_cast<const Location*>(route);

	//uri "/etc"
	//location /etc

	if (!base_path.empty()) {
		if (base_path.length() == 1) {
			resource = uri;
		}
		else if (base_path.length() == uri.length())
			resource = "";
		else {
			resource = uri.substr(base_path.length() + 1);
		}
	}
	std::cout << "RESOURCE=" << resource << std::endl;
	if (test) {
		if (!test->getAlias().empty()) {
			base_path = test->getAlias();
			return (joinPaths(base_path, resource));
		}
		else {
			// base_path.insert(0, rootPath);
			rootPath = route->getRoot();
			base_path.insert(0, rootPath);
		}
	}
	else {
		if (route->getRoot().empty())
			return "";
		rootPath = route->getRoot();
		base_path.insert(0, rootPath);
	}

	return (joinPaths(rootPath, uri));
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
		if (errno == EACCES)
			return (RESOURCE_FORBIDDEN);
		return (RESOURCE_NOT_FOUND);
	}

	if (S_ISDIR(fileInfo.st_mode))
		return (RESOURCE_DIRECTORY);
	if (S_ISREG(fileInfo.st_mode))
		return (RESOURCE_FILE);

	return (RESOURCE_FORBIDDEN);
}

/**
 * @brief Resolves a URI to a concrete physical path, including directory index files.
 * * If the resolved path is a directory, it iterates through the configured 
 * index files (e.g., "index.html") to find an existing file match.
 */
std::string ResourceLocator::resolvePath(const std::string& basePath, const RouteConfig* route) const {

	if (getResourceType(basePath) == RESOURCE_DIRECTORY) {
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
