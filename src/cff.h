#pragma once
#include <vector>
#include <list>
#include <functional>

namespace cff
{

template<typename ValueType, template<typename , typename> typename Container = std::vector , template<typename> typename Allocator = std::allocator>
class Enumerator
{
public:
	using Self = Enumerator< ValueType, Container, Allocator>;
	using Pred = std::function<bool(const ValueType&)>;

	template<typename Res>
	using Mapper = std::function<Res(const ValueType&)>;

	template<typename Res, typename T>
	using Zipper = std::function<Res(const ValueType&, const T&)>;

	using C = Container<ValueType, Allocator<ValueType> >;

	Self filter(Pred p)
	{
		Self result;
		auto now = result.mContainer.begin();

		for (auto& v : mContainer)
		{
			if (p(v))
			{
				now = ++result.mContainer.insert(now, v);
			}
		}

		return result;
	}

	template<typename Res>
	Enumerator<Res, Container, Allocator> map(Mapper<Res> m)
	{
		Enumerator<Res, Container, Allocator> result;
		auto now = result.container().begin();

		for (auto& v : mContainer)
		{
			now = ++result.container().insert(now, std::move(m(v)));
		}

		return result;
	}

	template<typename T>
	Enumerator<std::pair<ValueType, T>, Container, Allocator> zip(Enumerator<T, Container, Allocator> other)
	{
		auto f = mContainer.begin();
		auto s = other.container().begin();
		Enumerator<std::pair<ValueType, T>, Container, Allocator> result;
		auto now = result.container().begin();

		for (; f != mContainer.end() && s != other.container().end(); ++f, ++s)
		{
			now = ++result.container().insert(now, std::make_pair(*f, *s));
		}

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