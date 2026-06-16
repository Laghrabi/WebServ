#ifndef _MIMETYPESEXT_H
#define _MIMETYPESEXT_H
#include "webserver.hpp"

// NOTE: extentions are case insensitive
// so if you have file.htMl and types has
// html extention so the file will has 
// the mime_types that is associated with
// the extention

struct MimeTypesExt : public std::map<std::string, std::string> {
	public:
		typedef std::map<std::string, std::string> MAP;
		typedef std::vector<token>::iterator ContIter;
		typedef void (MimeTypesExt::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler;

		void parseMimeType(ContIter &begin);
};

#endif
