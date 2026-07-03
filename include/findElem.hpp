#ifndef _FINDELEM_H
#define _FINDELEM_H

#include <iterator>

template <typename T, typename ValueType> bool elemExist(const T& cont, const ValueType& value) {
	typename T::const_iterator end = cont.end();
	return (std::find(cont.begin(), end, value) != end);
}


template <typename T, typename ValueType> bool elemExist(const T& begin, const T& end,const ValueType& value) {
	return (std::find(begin, end, value) != end);
}

#endif
