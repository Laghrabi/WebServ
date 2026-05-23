#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include <exception>
class ParseConfig {
	void parseSimpleDir();
	void parseServerDir();
	void parseServer();

	class ConfigExcept : std::exception
	{

	};
};

#endif
