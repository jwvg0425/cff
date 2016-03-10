#pragma once
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <iterator>

namespace cff
{

//enumerator

template<typename ValueType, template<typename, typename> typename Container = std::vector, template<typename> typename Allocator = std::allocator>
class Enumerator
{
public:
	using Self = Enumerator<ValueType, Container, Allocator>;
	using C = Container<ValueType, Allocator<ValueType> >;

	Enumerator<ValueType, Container, Allocator>() = default;
	
	Enumerator<ValueType, Container, Allocator>(const Self& s)
		: mContainer(s.mContainer)
	{
	}
	
	Enumerator<ValueType, Container, Allocator>(Self&& s)
		: mContainer(std::move(s.mContainer))
	{
	}

	Enumerator<ValueType, Container, Allocator>(std::initializer_list<ValueType> init)
		: mContainer(init)
	{
	}

	Enumerator<ValueType, Container, Allocator>(const C& c)
		: mContainer(c)
	{
	}

	Enumerator<ValueType, Container, Allocator>(C&& c)
		: mContainer(std::move(c))
	{
	}

	Self& operator=(const Self& s)
	{
		mContainer.assign(s.mContainer);
	}

	Self& operator=(Self&& s)
	{
		mContainer.assign(std::move(s.mContainer));
	}


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

	template<typename Res, typename Fold>
	Res foldl(Res seed, Fold f)
	{
		foreach([&f, &seed](const ValueType& element) { f(seed, element); });

		return seed;
	}

	template<typename Res, typename Fold>
	Res foldr(Res seed, Fold f)
	{
		std::for_each(mContainer.rbegin(), mContainer.rend(),
			[&f, &seed](const ValueType& element) { f(element, seed); });

		return seed;
	}

	template<typename Pred>
	ValueType& find(Pred p)
	{
		return *std::find_if(mContainer.begin(), mContainer.end(), p);
	}

	template<typename Pred>
	Self filter(Pred p)
	{
		Self result;

		std::copy_if(mContainer.begin(), mContainer.end(), std::back_inserter(*result), p);
		return result;
	}

	template<typename comp>
	void sort()
	{
		std::sort(mContainer.begin(), mContainer.end());
	}

	template<typename Pred>
	void sort(Pred p)
	{
		std::sort(mContainer.begin(), mContainer.end(), p);
	}

	template<typename Mapper>
	auto map(Mapper m)
	{
		Enumerator<decltype(m(std::declval<ValueType>())), Container, Allocator> result;
		std::transform(mContainer.begin(), mContainer.end(), std::back_inserter(*result), m);

		return result;
	}

	template<typename For>
	void foreach(For f)
	{
		std::for_each(mContainer.begin(), mContainer.end(), f);
	}

	C& container()
	{
		return mContainer;
	}

	C* operator->()
	{
		return &mContainer;
	}

	C& operator*()
	{
		return container();
	}

private:
	C mContainer;
};


//curry

template<typename Lambda>
struct lambdaToFunc;

template <typename Res, typename Class, typename ...Args>
struct lambdaToFunc<Res(Class::*)(Args...) const>
{
	using funcType = std::function<Res(Args...)>;
};

template<typename Res>
std::function<Res()> curry(std::function<Res()> func)
{
	return func;
}

template<typename Res, typename Arg>
std::function<Res(Arg)> curry(std::function<Res(Arg)> func)
{
	return func;
}

template<typename Res, typename Arg1, typename Arg2, typename ...Args>
auto curry(std::function<Res(Arg1, Arg2, Args...)> func)
{
	return [func](Arg1&& arg1)
	{
		return curry<Res, Arg2, Args...>((std::function<Res(Arg2, Args...)>)
			[func, arg1](Arg2&& arg2, Args&&... args)
		{
			return func(arg1, std::forward<Arg2>(arg2), std::forward<Args>(args)...);
		});
	};
}

template<typename Lambda>
auto curry(Lambda&& lambda)
{
	return curry((lambdaToFunc<decltype(&Lambda::operator())>::funcType)std::forward<Lambda>(lambda));
}

}