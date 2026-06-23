#ifndef _UTIL_H
#define _UTIL_H
#include <iterator>
namespace util {

template <typename Iter> Iter next(Iter it) {
	std::advance(it);
	return (it);
}

}

#endif
