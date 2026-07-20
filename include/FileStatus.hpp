#ifndef _FILESTATUS_H
#define _FILESTATUS_H

#include <sys/stat.h>
#include <string>

class FileStatus {
	private:
		bool m_is_dir;
		bool m_exist;
	public:
		FileStatus();
		FileStatus(const std::string& str);
		void set(const std::string& str);
		bool exist() const;
		bool isDir() const;
};

#endif
