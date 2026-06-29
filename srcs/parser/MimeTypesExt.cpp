#include "MimeTypesExt.hpp"
#include "ParseConfig.hpp"

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
