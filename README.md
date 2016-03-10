# cff

각종 high order function을 쉽게 쓸 수 있게 도와주는 함수들, 클래스들 모음입니다.

## Requirements

- visual studio 2015+

## 사용법

 src 폴더의 cff.h 파일을 프로젝트에 포함시키면 됩니다.

## Enumerator

Enumerator는 vector, list 등의 순회 가능한 선형 컨테이너에 대해 map, filter, zipWith 등의 함수를 제공해주는 래핑 클래스입니다.

```C++
Eumerator<ValueType, Container, Allocator>
```

- ValueType : 해당 컨테이너에 담기는 값의 타입입니다.
- Container : vector, list 등의 선형 컨테이너 타입입니다. 생략시 std::vector로 지정됩니다.
- Allocator : 메모리 할당에 사용하는 allocator 타입입니다. 생략시 std::allocator로 지정됩니다.

### 제공 함수

```C++
using Self = Enumerator< ValueType, Container, Allocator>;
using C = Container<ValueType, Allocator<ValueType> >;

bool all(Pred p);
bool any(Pred p);
Res foldl(Res seed, Fold f);
Res foldr(Res seed, Fold f);
ValueType& find(Pred p);
Self filter(Pred p);
void sort();
void sort(Pred p);
auto map(Mapper m);
void foreach(For f);
auto zipWith(Zipper zipper, E1 e1, Es... es);
auto zip(E1 e1, Es... es);
C& container();
C* operator->();
C& operator*();
```

#### all
인자로 넘어가는 Pred 함수는 ValueType의 인자를 받아 bool 타입을 리턴하는 함수여야합니다. 내부 컨테이너의 모든 객체가 해당 조건을 만족하면 true, 아니면 false를 리턴합니다.

#### any
인자로 넘어가는 Pred 함수는 all과 마찬가지 조건을 만족하는 함수여야 합니다. 내부 컨테이너의 객체중 하나라도 해당 조건을 만족하면 true, 아니면 false를 리턴합니다.

```C++
cff::Enumerator<int> ei = {2,4,6,8,10};

ei.all([](int i){return i%2==0;}); // true
ei.any([](int i){return i%2==1;}); // false
```

#### foldl
시드 값과 누적 함수를 인자로 받습니다. Seed 타입은 무엇이든 상관 없고, Fold 타입은 Seed 타입의 값과 ValueType의 값을 인자로 받는 함수(Seed(Seed,ValueType))여야 합니다. 컨테이너의 맨 처음부터 시작해서 순서대로 누적 함수(Fold)를 적용시켜 결과값을 만들어냅니다.

#### foldr
 시드 값과 누적 함수를 인자로 받습니다. foldl과 마찬가지지만 Fold 타입은 Seed(ValueType,Seed)이야야 하고 컨테이너의 맨 마지막부터 시작해서 역순으로 누적 함수를 적용시켜 결과값을 만들어냅니다.

```C++
cff::Enumerator<int> ei = {1,2,3,4,5,6,7,8,9,10};

ei.foldl(0, [](int& acc, int i) { acc += i; }); // 55
// {10,9,8,7,6,5,4,3,2,1}
ei.foldr(cff::Enumerator<int>(), [](int i, cff::Enumerator<int>& acc) { acc->push_back(i); }); 
```

#### find 
Pred는 all과 마찬가지 조건을 만족하는 함수여야합니다. 해당 조건을 만족하는 맨 첫 번째 원소를 리턴합니다. 

```C++
cff::Enumerator<int> ei = {1,2,3,4,5,6,7,8,9,10};

ei.find([](int i) { return i == 5;}); // 5
```

#### filter
Pred는 all과 마찬가지 조건을 만족하는 함수여야합니다. 해당 조건을 만족하는 원소만 남긴 새로운 Enumerator를 리턴합니다.

```C++
cff::Enumerator<int> ei = {1,2,3,4,5,6,7,8,9,10};

ei.filter([](int i) { return i % 3 == 0; }); // {0,3,6,9}
```

#### sort
해당 리스트를 정렬합니다. Pred 인자가 넘어올 경우 해당 함수를 이용해 정렬합니다. 함수는 2개의 ValueType을 받아 첫번째 인자가 두번째 인자보다 컨테이너에서 앞에 위치해야한다면 true, 아니면 false를 반환하는 함수입니다.

```C++
cff::Enumerator<int> ei = {1,5,2,3,4};

ei.sort(); //{1,2,3,4,5}
ei.sort([](int i, int j) { return i>j; }); // {5,4,3,2,1}
```

#### map
Mapper는 ValueType을 인자로 받아 어떤 결과 값을 만들어내는 함수여야합니다. 해당 함수를 내부 컨테이너의 모든 원소에 적용시켜 새로운 Enumerator를 만들어냅니다.

```C++
cff::Enumerator<int> ei = {1,2,3};

ei.map([](int i) { return std::to_string(i);}); // {"1","2","3"}
```

#### foreach
For는 ValueType을 인자로 받는 함수여야합니다. 컨테이너의 모든 원소를 순회하며 해당 함수를 수행합니다.

```C++
cff::Enumerator<int> ei = {1,2,3,4,5};

ei.foreach([](int i) { printf("%d",i);});
```

#### zipWith
두 개 이상의 Enumerator를 합쳐 하나의 Enumerator로 만듭니다. 첫 번째 인자는 리스트를 합칠 때 사용할 함수이고, 두 번째 이후 인자는 합칠 Enumerator들의 목록입니다.

```C++
cff::Enumerator<int> one = { 1,2,3,4,5,6 };
cff::Enumerator<int> two = { 6,5,4,3,2,1 };
cff::Enumerator<int> three = { 1,2,3,4,5,6,7 };

//{161, 252, 343, 434, 525, 616} 
auto zipped = one.zipWith([](int a, int b, int c) { return a * 100 + b * 10 + c; }, two, three);
```

#### zip
두 개 이상의 Enumerator를 순차적으로 합친 튜플로 구성된하나의 Enumerator로 만듭니다.

```C++
cff::Enumerator<int> one = { 1,2,3,4,5,6 };
cff::Enumerator<int> two = { 6,5,4,3,2,1 };
cff::Enumerator<int> three = { 1,2,3,4,5,6,7 };

// {(1,6,1), (2,5,2), (3,4,3), (4,3,4), (5,2,5), (6,1,6)}
auto zipped = one.zip(two, three);

zipped.foreach([](std::tuple<int, int, int> t) 
{
 printf("(%d, %d, %d) ", 
  std::get<0>(t), 
  std::get<1>(t), 
  std::get<2>(t)); 
});
```


#### container
내부 컨테이너의 레퍼런스를 돌려줍니다. 내부 컨테이너에는 *, -> 연산자를 이용해서 접근할 수도 있습니다.


## Currying

curry 함수를 이용하여 일반 함수를 커리된 함수(curried function)로 만들 수 있습니다.

```C++
int plus(int x, int y)
{
 return x+y;
}

auto curriedPlus = cff::curry(plus);

curriedPlus(3)(5); // 8
auto plus7 = curriedPlus(7);

plus7(9); // 16

auto curried = cff::curry([](int x, int y) { return x == y; });
auto equal3 = curried(3);

equal3(5); // false
equal3(3); // true

```
