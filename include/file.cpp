struct iterator {

		typename ContIter m_cont_iter;
		typename ContIter m_cont_end;
		typename KeyValues::ValuesCont::iterator m_values_iter;

		std::pair<Key, Value> m_key_value_pair;

		iterator() {}

		iterator(const ContIter& cont_iter, const ContIter& cont_end, const ValuesContIter& values_iter) :
			m_cont_iter(cont_iter),
			m_cont_end(cont_end),
			m_values_iter(values_iter){
			}
		iterator(const ContIter& cont_iter, const ContIter& end_iter) : 
			m_cont_iter(cont_iter),
			m_cont_end(end_iter) {
			}

		iterator(const iterator& other) :
			m_cont_iter(other.m_cont_iter),
			m_cont_end(other.m_cont_end) {
				if (m_cont_end != m_cont_iter) {
					m_values_iter = other.m_values_iter;
				}
			}


		iterator& operator++() {
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

		iterator operator++(int) {
			iterator tmp = *this;
			++*this;
			return (tmp);
		}

		const Key& getKey()	{
			return (m_values_iter->m_key);
		}

		bool operator==(const iterator& other) const{
			(void)other;
			return (true);
			// return (m_cont_iter == other.m_cont_iter && 
			// 		m_values_iter == other.m_values_iter );
		}

		bool operator!=(const iterator& other) const{
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

