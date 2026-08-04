#include "webserver.hpp"

/* NOTE: 
 * this function takes an iterator
 * and adds a new keys(extentions) and values as (mime type) ex: key: application/html, value: .html
 * from *begin->value
 * INFO: 
 * MimeTypesExt is inherited from std::map<std::string, std::string>
 * MimeTypesExt can contain a lot of keys(extentions) and similar mime_type(ex: application/html html htm)
 */
void MimeTypesExt::parseMimeType(ContIter &begin) {
	std::string type = begin->value;
	std::string err_msg;

	if (begin->is_eof() || !begin->is(WORD)) {
		err_msg = "";
		// TODO: change that msg to indicate extention have not been provided
		throw (ParseConfig::ConfigExcept("expected " + type + " simple directive value", begin->line));
	}

	while (begin->is(WORD)) {
		std::string ext = begin->value;
		if (find(ext) != MAP::end())
		{
			throw (ParseConfig::ConfigExcept("duplacate extention: " + ext, begin->line));
		}
		operator[](ext) = type;
		++begin;
	}
}

std::string MimeTypesExt::getMimeType(const std::string& path) const{
	size_t dot = path.rfind('.');
    size_t slash = path.rfind("/");
	if (dot == std::string::npos || (slash != std::string::npos && dot < slash))
    {
		std::cout << "3lach dkhalt hna" << std::endl;
        return "application/octet-stream";
    }
    std::string ext = path.substr(dot + 1);
	std::cout << "this is the extention " << ext << std::endl;
	MAP::const_iterator it = find(ext);
	if (it == end()) {
		return ("application/octet-stream");
	}
	return (it->second);
}
