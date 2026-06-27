struct None {
	int a;
};

int main(void){
	struct None b;
	b.a = 2;
	struct None c;
	c.a = 3;
	printf("%d\n", b == c);
	return (0);
}
