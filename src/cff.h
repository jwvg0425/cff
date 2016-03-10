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

	template<typename Zipper, typename E1, typename ...Es>
	auto zipWith(Zipper zipper, E1 e1, Es... es)
	{
		auto begins = std::make_tuple(mContainer.begin(), e1->begin(), (es->begin())...);
		auto ends = std::make_tuple(mContainer.end(), e1->end(), (es->end())...);

		using Res = decltype(zipper(std::declval<ValueType>(), *e1->begin(), (*es->begin())...));
		Enumerator<Res, Container, Allocator> result;
		

		for (auto it = std::back_inserter(*result);
			!tupleIt<sizeof...(es)+1>::anyEqual(begins, ends);
			tupleIt<sizeof...(es)+1>::next(begins))
		{
			it = tupleIt<sizeof...(es)+1>::eval(zipper, begins);
		}

		return result;
	}

	template<typename E1, typename ...Es>
	auto zip(E1 e1, Es... es)
	{
		return zipWith([](auto&&... params)
		{
			return std::make_tuple(params...);
		}, e1, es...);
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

	template<int idx>
	struct tupleIt
	{
		template<typename ...Args>
		static bool anyEqual(const std::tuple<Args...>& t1, const std::tuple<Args...>& t2)
		{
			if (std::get<idx>(t1) == std::get<idx>(t2))
				return true;

			return tupleIt<idx - 1>::anyEqual(t1, t2);
		}

		template<typename ...Args>
		static void next(std::tuple<Args...>& t)
		{
			tupleIt<idx - 1>::next(t);

			++std::get<idx>(t);
		}

		template<typename Func, typename Tuple, typename...Args>
		static auto eval(Func func, Tuple&& t, Args&&... args)
		{
			return tupleIt<idx - 1>::eval(func, std::forward<Tuple>(t), std::get<idx>(t), args...);
		}
	};

	template<>
	struct tupleIt<0>
	{
		template<typename ...Args>
		static bool anyEqual(const std::tuple<Args...>& t1, const std::tuple<Args...>& t2)
		{
			return std::get<0>(t1) == std::get<0>(t2);
		}

		template<typename ...Args>
		static void next(std::tuple<Args...>& t)
		{
			++std::get<0>(t);
		}

		template<typename Func, typename Tuple, typename...Args>
		static auto eval(Func func, Tuple&& t, Args&&... args)
		{
			return func(*std::get<0>(t), (*args)...);
		}
	};

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