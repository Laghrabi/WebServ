template <typename Container> void print(Container cont) {
	typename Container::iterator it = cont.begin();
	while (it != cont.end())
	{
		std::cout << *it << "\n";
		it++;
	}
}
