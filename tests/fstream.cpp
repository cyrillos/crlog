#include <iostream>	/* cerr */
#include <fstream>
#include <stdlib.h>     /* atol */

size_t niter = 1000000;

int main(int argc, char *argv[])
{
	std::fstream f;
	const char str1[] = "String1 String1";
	static const char str2[] = "string2 string2 string2";

	if (argc == 2)
		niter = atol(argv[1]);

	if (argc > 2 || niter <= 0) {
		std::cerr << "Use: " << argv[0] << " [iter]\n";
		return 1;
	}

	std::ios::sync_with_stdio(false);

	f.open("/tmp/crlog.fstream.txt",
			std::fstream::out | std::fstream::trunc);

	for (int i = 0; i < niter; i++) {
		f << "Some message " << str1 << ' ' << str2 << ' '
			<< 'c' << ' ' << (long)-4 << ' '
			<< (short)2 << ' ' << (unsigned long)2 << std::endl;
	}

	f.close();
	return 0;
}
