#include "../src/cff.h"
#include <string>

int main()
{
	cff::Enumerator<int> vi = { 1,2,3,4,5,6,7,8,9,10 };

	//filter
	auto filtered = vi.filter([](int i) { return i % 2 == 0; });

	puts("=========== filter ==========");
	filtered.foreach([](int i) { printf("%d ", i); });
	puts("");

	//map
	auto mapped = vi.map([](int i) { return "number " + std::to_string(i); });

	puts("=========== map ==========");
	mapped.foreach([](const std::string& s) { printf("%s ", s.c_str()); });
	puts("");

	//all
	bool isAllEven = filtered.all([](int i) { return i % 2 == 0; });

	puts("=========== all ==========");
	printf("%s", isAllEven ? "true" : "false");
	puts("");

	//any
	bool isAnyOdd = vi.any([](int i) { return i % 2 == 1; });

	puts("=========== all ==========");
	printf("%s", isAnyOdd ? "true" : "false");
	puts("");
	
	//foldl
	auto sum = vi.foldl(0, [](int& acc, int i) { acc += i; });

	puts("=========== foldl ==========");
	printf("sum = %d", sum);
	puts("");

	//foldr
	auto reverse = vi.foldr(cff::Enumerator<int>(), [](int i, cff::Enumerator<int>& v) {v->push_back(i); });

	puts("=========== foldr ==========");
	reverse.foreach([](int i) { printf("%d ", i); });
	puts("");

	vi->push_back(3);

	auto curried = cff::curry([](int x, int y) {return x + y; });
	auto curried3 = curried(3);

	printf("%d %d", curried(5)(6), curried3(9));

	return 0;
}