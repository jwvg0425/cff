#include "../src/cff.h"

int main()
{
	cff::Enumerator<int> vi;
	cff::Enumerator<float> vf;

	for (int i = 0; i < 100; i++)
	{
		vi.container().push_back(i);
		vf.container().push_back((float)i*0.05f);
	}

	auto zip = vi.zip(vf);

	for (const auto& val : zip.container())
	{
		printf("(%d, %f) ", val.first, val.second);
	}

	auto filter = vi.filter([](int i) { return i % 3 == 0; });

	for (const auto& val : filter.container())
	{
		printf("%d ", val);
	}

	auto map = vf.map<int>([](float f) { return (int)(f * 100); });

	for (const auto& val : map.container())
	{
		printf("%d ", val);
	}

	return 0;
}