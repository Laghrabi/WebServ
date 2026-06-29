#include "webserver.hpp"
#include <utility>

template <class Key, class Value> struct UnorderedMultiMap {
	private:
	struct KeyValues {
		typedef std::deque<Value> ValuesCont;
		typedef typename std::deque<Value>::iterator iterator;
		typedef typename std::deque<Value>::const_iterator const_iterator;
		// std::pair<const Key, Value> m_pair;
		const Key m_key;
		ValuesCont m_values;

		typename ValuesCont::iterator m_it;

		public:
		KeyValues(const Key& key) :m_key(key){}
		const Key& getKey() const {
			return (m_key);
		}
		void insert(const Value& value) {
			m_values.push_back(value);
		}
		void print() const {
			std::cout << "key = " << m_key << "\n";
			for (typename ValuesCont::const_iterator it = m_values.begin(); it != m_values.end(); ++it) {
				// std::cout << m_key << ": " << *it << "\n";
			}
		}
	};

	public:
	std::list<KeyValues> m_cont_pair;
	typedef std::list<KeyValues> KeyValuesCont;

	typedef typename KeyValuesCont::iterator KeyValuesIter;
	typedef typename KeyValuesCont::iterator ContIter;
	typedef typename KeyValuesCont::const_iterator KeyValuesConstIter;
	typedef typename KeyValuesCont::const_iterator ConstIter;
	typedef typename KeyValues::iterator ValuesIter;
	typedef typename KeyValues::const_iterator ValuesConstIter;

	// typedef typename KeyValuesCont::iterator KeyValuesContIter;
	// typedef typename KeyValuesCont::iterator KeyValuesContConstIter;

	bool keyExist(const Key& key) const {
		for (typename KeyValuesCont::const_iterator it = m_cont_pair.begin(); it != m_cont_pair.end(); ++it) {
			if (it->getKey() == key)
				return (true);
		}
		return (false);
	}

	typename KeyValuesCont::const_iterator getIter(const Key& key) const{
		for (ConstIter it = m_cont_pair.begin(); it != m_cont_pair.end(); ++it) {
			if (key == it->getKey()) {
				return (it);
			}
		}
		return (m_cont_pair.end());
	}

typename KeyValuesCont::iterator getConstIter(const Key& key) {
		for (ContIter it = m_cont_pair.begin(); it != m_cont_pair.end(); ++it) {
			if (key == it->getKey()) {
				return (it);
			}
		}
		return (m_cont_pair.end());
	}


	void insert(const Key& key, const Value& value) {
		// std::cout << "new one inserted\n";
		if (keyExist(key)) {
			typename KeyValuesCont::iterator key_values = getConstIter(key);
			key_values->insert(value);
			// std::cout << "the key exist WORN WORN " << key << "\n";;
		}
		else {
			KeyValues item(key);
			item.insert(value);
			m_cont_pair.push_back(item)	;
			// std::cout << "KeyValues insert something: ";
			// std::cout << item.m_key << "\n";
			// std::cout << "size = " << m_cont_pair.size() << "\n";
		}
	}

	void insert(const std::pair<Key, Value>& pair) {
		insert(pair.first, pair.second);
	}

	void print(void) const {
		for (typename KeyValuesCont::const_iterator it = m_cont_pair.begin();
				it != m_cont_pair.end(); ++it) {
			it->print();
		}
	}


	struct const_iterator {

		typename KeyValuesCont::const_iterator m_cont_iter;
		typename KeyValuesCont::const_iterator m_cont_end;
		typename KeyValues::ValuesCont::const_iterator m_values_iter;

		std::pair<Key, Value> m_key_value_pair;

		const_iterator() {}

		const_iterator(const ConstIter& cont_iter, const ConstIter& cont_end, const ValuesConstIter& values_iter) :
			m_cont_iter(cont_iter),
			m_cont_end(cont_end),
			m_values_iter(values_iter){
			}
		const_iterator(const ConstIter& cont_iter, const ConstIter& end_iter) : 
			m_cont_iter(cont_iter),
			m_cont_end(end_iter) {
			}

		const_iterator(const const_iterator& other) :
			m_cont_iter(other.m_cont_iter),
			m_cont_end(other.m_cont_end) {
				if (m_cont_end != m_cont_iter) {
					m_values_iter = other.m_values_iter;
				}
			}


		const_iterator& operator++() {
			++m_values_iter;
			if (m_values_iter == m_cont_iter->m_values.end()) {
				// std::cout << "wa3y\n";
				++m_cont_iter;
				if (m_cont_iter != m_cont_end) {
					m_values_iter = m_cont_iter->m_values.begin();
				}
			}
			return (*this);
		};

		const_iterator operator++(int) {
			const_iterator tmp = *this;
			++*this;
			return (tmp);
		}

		const Key& getKey()	{
			return (m_values_iter->m_key);
		}

		bool operator==(const const_iterator& other) const{
			(void)other;
			return (true);
			// return (m_cont_iter == other.m_cont_iter && 
			// 		m_values_iter == other.m_values_iter );
		}

		bool operator!=(const const_iterator& other) const{
			// std::cout << "first " << (m_cont_iter == other.m_cont_iter) << "\n";
			// std::cout << "second " << (m_values_iter == other.m_values_iter) << "\n";
			// std::cout << "equal end " << (m_cont_iter == other.m_cont_end) << "\n";
			if (m_cont_iter == other.m_cont_iter && other.m_cont_iter == m_cont_end)
				return (false);
			return (m_values_iter != other.m_values_iter);
		}
		void nextBound() {
			++m_cont_iter;
			m_values_iter = m_cont_iter->m_values.begin();
		}
			std::pair<Key, Value>* operator->() {
			m_key_value_pair.first = m_cont_iter->m_key;
			m_key_value_pair.second = *m_values_iter;
			return (&m_key_value_pair);
		}
	};

	const_iterator begin() const{
		if (m_cont_pair.begin() == m_cont_pair.end())
			return (end());
		return (const_iterator(m_cont_pair.begin(), m_cont_pair.end(), m_cont_pair.begin()->m_values.begin()));
	}

	const_iterator end() const{
		return (const_iterator(m_cont_pair.end(), m_cont_pair.end()));
	}

	const_iterator upper_bound(const Key& key) const{
		ConstIter cont_end = m_cont_pair.end();
		if (keyExist(key)) {
			ConstIter it = getIter(key);
			++it;
			if (it == cont_end)
				return (end());;
			return (const_iterator(it, cont_end, it->m_values.begin()));
		}
		return (end());
	}

	std::size_t size() const{
		return (m_cont_pair.size());
	}

	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const{
		ConstIter it = getIter(key);
		if (it != m_cont_pair.end()) {
			std::pair<const_iterator, const_iterator> pair;
			pair.first = const_iterator(it, m_cont_pair.end(), it->m_values.begin());
			it++;
			if (it == m_cont_pair.end()) {
				pair.second = end();
			}
			else {
				pair.second = const_iterator(it, m_cont_pair.end(), it->m_values.begin());
			}
			return (pair);
		}
		return (std::make_pair(end(), end()));
	}

};


