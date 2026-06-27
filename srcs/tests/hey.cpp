#include<iostream>
struct None {
	int a;
};

int main(void){
	None b;
	b.a = 2;
	None c;
	c.a = 3;
	std::cout << (b == c) << "\n";
	return (0);
}
