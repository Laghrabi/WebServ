#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"
#include "MimeTypesExt.hpp"
#include "Server.hpp"

template <class Key, class Value> struct UnorderedMultiMap {
	struct KeyValues {
			typedef std::deque<Value> ValuesCont;
			const Key& m_key;
			ValuesCont m_values;

			typename ValuesCont::iterator m_it;
			// const Key first;
			// Value second;

		public:
			KeyValues(const Key& key) :m_key(key) {
				// first = m_key;
				// m_it = m_values.begin();
			}
			const Key& getKey() const {
				return (m_key);
			}
			void insert(const Value& value) {
				m_values.push_back(value);
			}
			void print() const {
				for (typename ValuesCont::const_iterator it = m_values.begin(); it != m_values.end(); ++it) {
					std::cout << m_key << ": " << *it << "\n";
				}
			}
			// void operator++() {
			// 	;
			// }
	};

	std::list<KeyValues> m_key_values_pair;
	typedef std::list<KeyValues> KeyValuesCont;

	bool keyExist(const Key& key) const {
		for (typename KeyValuesCont::const_iterator it = m_key_values_pair.begin(); it != m_key_values_pair.end(); ++it) {
			if (it->getKey() == key)
				return (true);
		}
		return (false);
	}

	typename KeyValuesCont::iterator getKeyValuesItem(const Key& key) {
		for (typename KeyValuesCont::iterator it = m_key_values_pair.begin(); it != m_key_values_pair.end(); ++it) {
			if (key == it->getKey()) {
				return (it);
			}
		}
		return (m_key_values_pair.end());
	}


	void insert(const Key& key, const Value& value) {
		if (keyExist(key)) {
			typename KeyValuesCont::iterator key_values = getKeyValuesItem(key);
			key_values->insert(value);
		}
		else {
			KeyValues item(key);
			item.insert(value);
			m_key_values_pair.push_back(item)	;
		}
	}

	void insert(const std::pair<Key, Value>& pair) {
		insert(pair.first, pair.second);
	}

	void print(void) const {
		for (typename KeyValuesCont::const_iterator it = m_key_values_pair.begin();
				it != m_key_values_pair.end(); ++it) {
			it->print();
		}
	}


	struct iterator {iterator upper_bound( const Key& key );
		typename KeyValues::ValuesCont::const_iterator m_values_iter_end;
		typename KeyValues::ValuesCont::const_iterator m_values_iter;
		typename KeyValuesCont::iterator m_key_values_iter;
		Key const *m_key;

		iterator(const typename KeyValuesCont::iterator& key_values_iter, typename KeyValues::ValuesCont* values_cont = NULL) {
			m_key_values_iter = key_values_iter;
			if (values_cont == NULL) {
				values_cont = &key_values_iter->m_values;
			}
			m_values_iter_end = values_cont->end();
			m_values_iter = values_cont->begin();
			m_key = &m_key_values_iter->m_key;
		}

		iterator& operator++() {
			++m_values_iter;
			if (m_values_iter == m_values_iter_end)
			{
				++m_key_values_iter;
				m_values_iter = m_key_values_iter->m_values.begin();
				m_values_iter_end = m_key_values_iter->m_values.end();
				m_key = &m_key_values_iter->m_key;
			}
			return (*this);
		};

		iterator operator++(int) {
			iterator tmp = *this;
			++*this;
			return (tmp);
		}
		const Key& getKey()	{
			return (*m_key);
		}
		const Value& getValue()	{
			return (*m_values_iter);
		}
		bool operator==(const iterator& other) const{
			return (m_values_iter == other.m_values_iter);
		}
		bool operator!=(const iterator& other) const{
			return (m_key_values_iter != other.m_key_values_iter);
		}
		void nextBound() {
			++m_key_values_iter;
			m_values_iter_end = m_key_values_iter->m_values.end();
			m_values_iter = m_key_values_iter->m_values.begin();
			m_key = &m_key_values_iter->m_key;
		}
	};


	iterator begin() {
		return (iterator(m_key_values_pair.begin()));
	}

	iterator end() {
		return (iterator(m_key_values_pair.end()));
	}

	iterator upper_bound(const Key& key) {
		if (keyExist(key)) {
			typename KeyValuesCont::const_iterator it = getKeyValuesItem(key);
			typename KeyValuesCont::const_iterator end = m_key_values_pair.end();
			++it;
			if (it == end)
				return (end());
		}
		return (end());
	}
};

struct Config {
	typedef std::list<Server> ServerCont;
	MimeTypesExt m_types;
	UnorderedMultiMap<Server::IPort, Server> m_iport_server;
	std::list<Server> m_servers;
};


void print(const Config&conf);

#endif
