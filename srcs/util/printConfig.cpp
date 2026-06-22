#include "Config.hpp"
#include "Server.hpp"
#include "webserver.hpp"
#include "tokenization.hpp"
#include "MimeTypesExt.hpp"

// Reset
#define COLOR_RESET        "\033[0m"

// Text Colors
#define COLOR_BLACK        "\033[30m"
#define COLOR_RED          "\033[31m"
#define COLOR_GREEN        "\033[32m"
#define COLOR_YELLOW       "\033[33m"
#define COLOR_BLUE         "\033[34m"
#define COLOR_MAGENTA      "\033[35m"
#define COLOR_CYAN         "\033[36m"
#define COLOR_WHITE        "\033[37m"

// Bright Text Colors
#define COLOR_BRIGHT_BLACK   "\033[90m"
#define COLOR_BRIGHT_RED     "\033[91m"
#define COLOR_BRIGHT_GREEN   "\033[92m"
#define COLOR_BRIGHT_YELLOW  "\033[93m"
#define COLOR_BRIGHT_BLUE    "\033[94m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_BRIGHT_CYAN    "\033[96m"
#define COLOR_BRIGHT_WHITE   "\033[97m"

// Background Colors
#define BG_BLACK           "\033[40m"
#define BG_RED             "\033[41m"
#define BG_GREEN           "\033[42m"
#define BG_YELLOW          "\033[43m"
#define BG_BLUE            "\033[44m"
#define BG_MAGENTA         "\033[45m"
#define BG_CYAN            "\033[46m"
#define BG_WHITE           "\033[47m"

// Bright Background Colors
#define BG_BRIGHT_BLACK    "\033[100m"
#define BG_BRIGHT_RED      "\033[101m"
#define BG_BRIGHT_GREEN    "\033[102m"
#define BG_BRIGHT_YELLOW   "\033[103m"
#define BG_BRIGHT_BLUE     "\033[104m"
#define BG_BRIGHT_MAGENTA  "\033[105m"
#define BG_BRIGHT_CYAN     "\033[106m"
#define BG_BRIGHT_WHITE    "\033[107m"

void print_types(const MimeTypesExt &mime, int level) {
	for (MimeTypesExt::const_iterator it = mime.begin(); it != mime.end(); it++) {
		std::cout << std::string(level, '\t') << COLOR_BLUE << it->first << COLOR_MAGENTA": " << it->second << "\n";
	}
}

template <typename T> T next(T it) {
	return (++it);
}

template <typename T> void print_dir(const T& directive, const std::string& directive_name, const int level) {
	if (directive.empty())
		return ;
	std::cout << std::string(level, '\t') << COLOR_BLUE << directive_name << COLOR_RESET << "\n";
	for (typename T::const_iterator it = directive.begin();it != directive.end(); it++) {
		std::cout << std::string(level + 1, '\t');
		if (next(it) != directive.end()) {
			std::cout << "├── '";
		}
		else {
			std::cout << "└── '";
		}
		std::cout << COLOR_GREEN << *it << COLOR_RESET"'\n";
	}
};

void print_dir(std::string directive, const std::string& str, int level) {
	if (!directive.empty())
		std::cout << std::string(level, '\t') << COLOR_BRIGHT_CYAN << str << ": " << directive << "\n";
}

void print_server(const Server<std::vector<token> >& server, int level) {
	(void)level;
	print_dir(server.m_root, "root", 2);
	print_dir(server.m_root, "upload directory", level);
	print_dir(server.m_autoindex ? std::string("on") : "off", "autoindex", level);

	print_dir(server.m_indexes, "index", level);
	print_dir(server.m_hosts, "server_names", level);
	print_dir(server.m_addr, "addr/port", level);
}

void print(const Config& conf) {
	// if (!conf.m_types.empty())
	// {
	// 	std::cout << "hey\n";
	// 	std::cout << COLOR_GREEN"global types: ";
	// 	print_types(conf.m_types, 1);
	// }
	for (std::vector<Server<std::vector<token> > >::const_iterator it = conf.m_servers.begin(); it != conf.m_servers.end(); ++it) {
		std::cout << COLOR_MAGENTA"new server\n"COLOR_RESET;
		print_server(*it, 1);
	}
}
