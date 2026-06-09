#include "MimeTypesExt.hpp"
#include "ParseConfig.hpp"

void MimeTypesExt::parseMimeType(ContIter &begin) {
	std::string type = (*begin).value;
	std::string err_msg;

	if ((*begin).is_eof() || !(*begin).is(WORD)) {
		err_msg = "";
		// TODO: change that msg to indicate extention have not been provided
		throw (ParseConfig<TokenCont>::ConfigExcept("expected " + type + " simple directive value", (*begin).line));
	}

	while ((*begin).is(WORD)) {
		std::string ext = (*begin).value;
		if (find(ext) != MAP::end())
		{
			throw (ParseConfig<>::ConfigExcept("duplacate extention: " + ext, (*begin).line));
		}
		operator[](ext) = type;
		++begin;
	}
}
