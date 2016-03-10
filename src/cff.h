#pragma once
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <iterator>

namespace cff
{

template<typename ValueType, template<typename , typename> typename Container = std::vector , template<typename> typename Allocator = std::allocator>
class Enumerator
{
public:
	using Self = Enumerator< ValueType, Container, Allocator>;
	using C = Container<ValueType, Allocator<ValueType> >;

	template<typename Pred>
	bool all(Pred p)
	{
		return std::all_of(mContainer.begin(), mContainer.end(), p);
	}

	template<typename Pred>
	bool any(Pred p)
	{
		return std::any_of(mContainer.begin(), mContainer.end(), p);
	}

	template<typename Pred>
	Self filter(Pred p)
	{
		Self result;

		std::copy_if(mContainer.begin(), mContainer.end(), std::back_inserter(result.container()), p);
		return result;
	}

	template<typename Mapper>
	auto map(Mapper m)
	{
		Enumerator<decltype(m(ValueType())), Container, Allocator> result;
		std::transform(mContainer.begin(), mContainer.end(), std::back_inserter(result.container()), m);

		return result;
	}

	C& container()
	{
		return mContainer;
	}

private:
	C mContainer;
};


}