#include "webserver.hpp"
#include <utility>

template <class Key, class Value> struct UnorderedMultiMap {
	private:
		struct KeyValues {
			typedef std::pair<const Key, Value> pair;
			typedef std::deque<pair > PairCont;
			typedef typename std::deque<pair>::iterator iterator;
			typedef typename std::deque<pair>::const_iterator const_iterator;

			const Key m_key;
			PairCont m_values;

			typename PairCont::iterator m_it;

			public:
			KeyValues(const Key& key, const Value& value) :
				m_key(key){
					m_values.push_back(pair(m_key, value));
				}
			const Key& getKey() const {
				return (m_key);
			}
			void insert(const Value& value) {
				m_values.push_back(pair(m_key, value));
			}
			void print() const {
				std::cout << "key = " << m_key << "\n";
				for (const_iterator it = m_values.begin(); it != m_values.end(); ++it) {
					std::cout << m_key << ": " << it->second << "\n";
				}
			}
		};

	public:
		std::list<KeyValues> m_cont_pair;

		typedef std::list<KeyValues> KeyValuesCont;

		typedef typename KeyValuesCont::iterator ContIter;
		typedef typename KeyValuesCont::const_iterator ConstIter;

		typedef typename KeyValues::iterator ValuesIter;
		typedef typename KeyValues::const_iterator ValuesConstIter;

		typedef typename KeyValues::pair pair;

		bool keyExist(const Key& key) const {
			for (typename KeyValuesCont::const_iterator it = m_cont_pair.begin(); it != m_cont_pair.end(); ++it) {
				if (it->getKey() == key)
					return (true);
			}
			return (false);
		}

		ConstIter getIter(const Key& key) const{
			for (ConstIter it = m_cont_pair.begin(); it != m_cont_pair.end(); ++it) {
				if (key == it->getKey()) {
					return (it);
				}
			}
			return (m_cont_pair.end());
		}
		ContIter getIter(const Key& key) {
			for (ContIter it = m_cont_pair.begin(); it != m_cont_pair.end(); ++it) {
				if (key == it->getKey()) {
					return (it);
				}
			}
			return (m_cont_pair.end());
		}


		void insert(const Key& key, const Value& value) {
			if (keyExist(key)) {
				typename KeyValuesCont::iterator key_values = getIter(key);
				key_values->insert(value);
			}
			else {
				KeyValues item(key, value);
				m_cont_pair.push_back(item)	;
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

		struct iterator {

			ContIter m_cont_iter;
			ContIter m_cont_end;
			ValuesIter m_pair_iter;


			iterator() {}

			iterator(const ContIter& cont_iter, const ContIter& cont_end, const ValuesIter& values_iter) :
				m_cont_iter(cont_iter),
				m_cont_end(cont_end),
				m_pair_iter(values_iter){
				}
			iterator(const ContIter& cont_iter, const ContIter& end_iter) : 
				m_cont_iter(cont_iter),
				m_cont_end(end_iter) {
				}

			iterator(const iterator& other) :
				m_cont_iter(other.m_cont_iter),
				m_cont_end(other.m_cont_end) {
					if (m_cont_end != m_cont_iter) {
						m_pair_iter = other.m_pair_iter;
					}
				}
			iterator& operator=(const iterator& other) {
				m_cont_iter = other.m_cont_iter;
				m_cont_end = other.m_cont_end;
				if (m_cont_end != m_cont_iter) {
					m_pair_iter = other.m_pair_iter;
				}
				return (*this);
			}


			iterator& operator++() {
				++m_pair_iter;
				if (m_pair_iter == m_cont_iter->m_values.end()) {
					// std::cout << "wa3y\n";
					++m_cont_iter;
					if (m_cont_iter != m_cont_end) {
						m_pair_iter = m_cont_iter->m_values.begin();
					}
				}
				return (*this);
			};

			iterator operator++(int) {
				iterator tmp = *this;
				++*this;
				return (tmp);
			}

			// const Key& getKey()	{
			// 	return (m_pair_iter->m_key);
			// }

			bool operator==(const iterator& other) const {
				(void)other;
				return (true);
				// return (m_cont_iter == other.m_cont_iter && 
				// 		m_pair_iter == other.m_pair_iter );
			}

			bool operator!=(const iterator& other) const {
				if (m_cont_iter == other.m_cont_iter && other.m_cont_iter == m_cont_end)
					return (false);
				return (m_pair_iter != other.m_pair_iter);
			}
			void nextBound() {
				++m_cont_iter;
				m_pair_iter = m_cont_iter->m_values.begin();
			}
			pair* operator->() {
				return (&*m_pair_iter);
			}
		};



		struct const_iterator {

			typename KeyValuesCont::const_iterator m_cont_iter;
			typename KeyValuesCont::const_iterator m_cont_end;
			typename KeyValues::PairCont::const_iterator m_pair_iter;


			const_iterator() {}

			const_iterator(const ConstIter& cont_iter, const ConstIter& cont_end, const ValuesConstIter& values_iter) :
				m_cont_iter(cont_iter),
				m_cont_end(cont_end),
				m_pair_iter(values_iter){
				}
			const_iterator(const ConstIter& cont_iter, const ConstIter& end_iter) : 
				m_cont_iter(cont_iter),
				m_cont_end(end_iter) {
				}

			const_iterator(const const_iterator& other) :
				m_cont_iter(other.m_cont_iter),
				m_cont_end(other.m_cont_end) {
					if (m_cont_end != m_cont_iter) {
						m_pair_iter = other.m_pair_iter;
					}
				}

			const_iterator& operator=(const const_iterator& other) {
				m_cont_iter = other.m_cont_iter;
				m_cont_end = other.m_cont_end;
				if (m_cont_end != m_cont_iter) {
					m_pair_iter = other.m_pair_iter;
				}
				return (*this);
			}



			const_iterator& operator++() {
				++m_pair_iter;
				if (m_pair_iter == m_cont_iter->m_values.end()) {
					++m_cont_iter;
					if (m_cont_iter != m_cont_end) {
						m_pair_iter = m_cont_iter->m_values.begin();
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
				return (m_pair_iter->m_key);
			}

			bool operator==(const const_iterator& other) const{
				(void)other;
				return (true);
				// return (m_cont_iter == other.m_cont_iter && 
				// 		m_pair_iter == other.m_pair_iter );
			}

			bool operator!=(const const_iterator& other) const{
				if (m_cont_iter == other.m_cont_iter && other.m_cont_iter == m_cont_end)
					return (false);
				return (m_pair_iter != other.m_pair_iter);
			}
			void nextBound() {
				++m_cont_iter;
				m_pair_iter = m_cont_iter->m_values.begin();
			}
			const pair* operator->() const{
				return (&*m_pair_iter);
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

		iterator begin() {
			if (m_cont_pair.begin() == m_cont_pair.end())
				return (end());
			return (iterator(m_cont_pair.begin(), m_cont_pair.end(), m_cont_pair.begin()->m_values.begin()));
		}

		iterator end() {
			return (iterator(m_cont_pair.end(), m_cont_pair.end()));
		}

		const_iterator upper_bound(const Key& key) const{
			// std::cout << "last chance: " << key << "\n";
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

		iterator upper_bound(const Key& key){
			ContIter cont_end = m_cont_pair.end();
			if (keyExist(key)) {
				ContIter it = getIter(key);
				++it;
				if (it == cont_end)
					return (end());;
				return (iterator(it, cont_end, it->m_values.begin()));
			}
			return (end());
		}

		std::size_t size() const{
			return (m_cont_pair.size());
		}

		std::pair<iterator, iterator> equal_range(const Key& key) {
			ContIter it = getIter(key);
			if (it != m_cont_pair.end()) {
				std::pair<iterator, iterator> pair;
				pair.first = iterator(it, m_cont_pair.end(), it->m_values.begin());
				it++;
				if (it == m_cont_pair.end()) {
					pair.second = end();
				}
				else {
					pair.second = iterator(it, m_cont_pair.end(), it->m_values.begin());
				}
				return (pair);
			}
			return (std::make_pair(end(), end()));
		}

		std::pair<const_iterator, const_iterator> equal_range(const Key& key) const{
			ConstIter it = getIter(key);
			if (it != m_cont_pair.end()) {
				std::pair<const_iterator, const_iterator> res;
				res.first = const_iterator(it, m_cont_pair.end(), it->m_values.begin());
				it++;
				if (it == m_cont_pair.end()) {
					res.second = end();
				}
				else {
					res.second = const_iterator(it, m_cont_pair.end(), it->m_values.begin());
				}
				return (res);
			}
			return (std::make_pair(end(), end()));
		}
};
