#include <unistd.h>

int safeClose(int fd) {
	if (fd > 0) {
		return (close(fd));
	}
	return (-1);
}
