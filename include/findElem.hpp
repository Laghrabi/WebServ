#ifndef _FINDELEM_H
#define _FINDELEM_H

#include <algorithm>

template <typename T, typename ValueType> bool elemExist(const T& cont, const ValueType& value) {
	typename T::const_iterator end = cont.end();
	return (std::find(cont.begin(), end, value) != end);
}

template <typename Cont, typename ValueType> typename Cont::const_iterator elemIter(const Cont& cont, const ValueType& value) {

	typename Cont::const_iterator end = cont.end();
	typename Cont::const_iterator begin = cont.begin();
	typename Cont::const_iterator it = std::find(begin, end, value);

	return (it);

}

// template <typename T, typename ValueType> T elemIter(const T& begin, const T& end, const ValueType& value) {
// 	T it = std::find(begin, end, value);
// 	return (it);
// }

template <typename T, typename ValueType> bool mapElemExist(const T& map, const ValueType& value) {
	return (map.find(value) != map.end());
}



template <typename T, typename ValueType> bool elemExist(const T& begin, const T& end,const ValueType& value) {
	return (std::find(begin, end, value) != end);
}

#endif
