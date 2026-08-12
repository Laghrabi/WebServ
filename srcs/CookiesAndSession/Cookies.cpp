#include "Cookies.hpp"

void Cookies::parse(const std::string& header)
{
	_cookies.clear();

	size_t start = 0;

	while (start < header.size())
	{
		size_t end = header.find(';', start);

		std::string cookie = header.substr(
			start,
			end == std::string::npos ? std::string::npos : end - start
		);

		cookie = trimSpaces(cookie);

		size_t equal = cookie.find('=');

		if (equal != std::string::npos)
		{
			std::string name = trimSpaces(cookie.substr(0, equal));
			std::string value = trimSpaces(cookie.substr(equal + 1));

			if (!name.empty())
				_cookies[name] = value;
		}

		if (end == std::string::npos)
			break;

		start = end + 1;
	}
}

bool Cookies::has(const std::string& name) const
{
	return (_cookies.find(name) != _cookies.end());
}

std::string Cookies::get(const std::string& name) const
{
	std::map<std::string, std::string>::const_iterator it;

	it = _cookies.find(name);

	if (it == _cookies.end())
		return "";

	return it->second;
}
