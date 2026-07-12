#include<iostream>
#include<sys/stat.h>

class hey {
	private:
		std::string str;
		bool is_dir;
		bool exist;
	public:
		hey() {

		}
		hey(const std::string& str) {
			;
		}
		void set(const std::string& str) {
			this->str = str;
			struct stat buf;
			if (stat(str.c_str(), &buf)) {
				is_dir = false;
				exist = false;
				return ;
			}
			exist = true;
			is_dir = S_ISDIR(buf.st_mode);
		}
		bool isExist() const {
			return (exist);
		}
		bool isDir() const {
			return (is_dir);
		}
};

enum REQUEST {
	CGI,
	NOT_FOUND,
	IS_FILE,
	IS_DIR
};


bool hasExtention(const std::string& path) {
	std::string ext = ".conf";
	std::size_t pos;
	std::size_t len = ext.length();

	pos = path.find_last_of('.');
	if (pos != std::string::npos)
	{
		if (path.length() - pos != len) // hey.conf lengh = 8; pos = 3; 5; .conf = 5
			return (false);
		return (path.compare(pos, len, ext) == 0);
	}
	return (false);
}

enum REQUEST isCgi(void) {


	std::string l[] = {"/bin", "/../", "/../", "/bi"};
	hey b;
	std::string path;
	for (int i = 0; i < sizeof(l) / sizeof(std::string); i++) {
		path += l[i];
		b.set(path);
		if (b.isExist()) {
			if (b.isDir()) {
				std::cout << path << " is dir\n";
				continue;
			}
			else if (hasExtention(path)){
				std::cout << path << " is file\n";
				return (IS_FILE);
			}
		}
		else {
			std::cout << path << " not found\n";
			return (NOT_FOUND);
		}
	}
	return (IS_DIR);
}



int main(void){
	std::cout << hasExtention("hey.conf") << "\n";
	switch (isCgi()) {
		case CGI:
			std::cout << "CGI";
			break;
		case NOT_FOUND:
			std::cout << "NOT_FOUND";
			break;
		case IS_FILE:
			std::cout << "IS_FILE";
			break;
		case IS_DIR:
			std::cout << "IS_DIR";
			break;
	}
	return (0);
}
