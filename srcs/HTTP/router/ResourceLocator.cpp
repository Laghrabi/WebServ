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

// void buildRoot(std::string& path, const std::string& root) {
// 	;
// }
//
// void buildAlias(std::string& path, const std::string& location, const std::string& alias) {
// 	;
// }

std::string ResourceLocator::buildPhysicalPath(const HttpRequest& request, std::string& base_path, std::string& resource) const {
	std::string rootPath = "";

	const RouteConfig* route = request._routeResult.route;
	std::string uri = request.getRouteUri();

	// bool base_path_slash = false;
	// bool resource_slash = false;

	const Location* test = dynamic_cast<const Location*>(route);

	//uri "/etc"
	//location /etc
	std::cout << "HELLO\n" << "base_path=" <<  base_path << std::endl << "uri=" << uri << std::endl;
	if (!base_path.empty()) {
		if (base_path.length() == 1 || base_path.length() == uri.length()) {
			resource = uri;
		}
		else {
			resource = uri.substr(base_path.length() + 1);
		}
	}
	std::cout << "RESOURCE=" << resource << std::endl;
	if (test) {
		if (!test->getAlias().empty()) {
			base_path = test->getAlias();
			// base_path_slash = base_path.at(base_path.length() - 1) == '/';
			// resource_slash = resource.at(resource.length() - 1) == '/';
			// if (base_path_slash)
			// 	return (base_path + resource);
			// return (base_path + "/" + resource);
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

	// bool rootEndsWithSlash = (rootPath[rootPath.length() - 1] == '/');
	// bool uriStartsWithSlash = (!uri.empty() && uri[0] == '/');

	// if (rootEndsWithSlash && uriStartsWithSlash)
	// {
	// 	return (rootPath + uri.substr(1));
	// }
	// else if (!rootEndsWithSlash && !uriStartsWithSlash) {
	// 	base_path = rootPath;
	// 	return (rootPath + "/" + uri);
	// }

	// return (rootPath + uri);
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
