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

	puts("=========== any ==========");
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

	//currying
	auto curried = cff::curry([](int x, int y) { return x == y; });
	auto equal3 = curried(3);

	puts("=========== foldr ==========");
	printf("%s", vi.any(equal3) ? "true" : "false");
	puts("");

	//zipWith
	cff::Enumerator<int> one = { 1,2,3,4,5,6 };
	cff::Enumerator<int> two = { 6,5,4,3,2,1 };
	cff::Enumerator<int> three = { 1,2,3,4,5,6,7 };

	auto zipped = one.zipWith([](int a, int b, int c) { return a * 100 + b * 10 + c; }, two, three);
	auto zipped2 = one.zip(two, three);

	puts("=========== zipWith ==========");
	zipped.foreach([](int i) {printf("%d ", i); });
	puts("");

	puts("=========== zip ==========");
	zipped2.foreach([](std::tuple<int, int, int> t) {printf("(%d, %d, %d) ", std::get<0>(t), std::get<1>(t), std::get<2>(t)); });
	puts("");

	//sort
	cff::Enumerator<int> sort = { 1,4,2,3,5 };

	sort.sort();

	puts("=========== sort ==========");
	sort.foreach([](int i) {printf("%d ", i); });
	puts("");

	sort.sort([](int i, int j) { return i > j; });

	sort.foreach([](int i) {printf("%d ", i); });
	puts("");
	
	return 0;
}