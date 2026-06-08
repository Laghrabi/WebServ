#ifndef _MIMETYPESEXT_H
#define _MIMETYPESEXT_H
#include "webserver.hpp"

struct MimeTypesExt : public std::map<std::string, std::string> {
	protected:
		std::string m_value;
	public:
		typedef std::map<std::string, std::string> MAP;
		typedef std::vector<token>::iterator ContIter;
		typedef void (MimeTypesExt::*HandlerFunc)(ContIter&,const ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler;


		void setValue(const std::string& value);
		void parseMimeType(ContIter &begin, const ContIter &end);
};

#endif
