#include "MimeTypesExt.hpp"
#include "ParseConfig.hpp"


void MimeTypesExt::setValue(const std::string& value) {
	m_value = value;
}

void MimeTypesExt::parseMimeType(ContIter &begin, const ContIter &end) {
	std::cout << "all clear\n";
	(void)end;
	while ((*begin).is(WORD)) {
		std::string ext = (*begin).value;
		if (find(ext) != MAP::end())
		{
			throw (ParseConfig<>::ConfigExcept("duplacate extention: " + ext, (*begin).line));
		}
		operator[](ext) = m_value;
		++begin;
	}
}
