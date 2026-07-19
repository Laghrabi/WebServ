#include "FileStatus.hpp"

FileStatus::FileStatus(void) :
m_is_dir(false),
m_exist(false){

}
FileStatus::FileStatus(const std::string& str) {
	set(str);
}

void FileStatus::set(const std::string& str) {
	struct stat buf;
	if (stat(str.c_str(), &buf)) {
		m_is_dir = false;
		m_exist = false;
		return ;
	}
	m_exist = true;
	m_is_dir = S_ISDIR(buf.st_mode);
}

bool FileStatus::exist() const {
	return (m_exist);
}

bool FileStatus::isDir() const {
	return (m_is_dir);
}
