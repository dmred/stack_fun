#include <iostream>
#include <new>  
#include <stdexcept>
#include <vector>
#include <memory>

// ╔══╗╔══╦════╦══╦═══╦════╗
// ║╔╗║╚╗╔╩═╗╔═╣╔═╣╔══╩═╗╔═╝
// ║╚╝╚╗║║──║║─║╚═╣╚══╗─║║
// ║╔═╗║║║──║║─╚═╗║╔══╝─║║
// ║╚═╝╠╝╚╗─║║─╔═╝║╚══╗─║║
// ╚═══╩══╝─╚╝─╚══╩═══╝─╚╝

class bitset
{
	public:
		explicit
		bitset(size_t size);/*strong*/
		bitset(bitset const &) = delete;
		bitset(bitset &&) = delete;
		auto operator=(bitset &&) -> bitset & = delete;
		auto operator =(bitset const & )->bitset & = delete;
		auto set(size_t index) -> void;/*strong*/
		auto reset(size_t index) -> void;/*strong*/
		auto test(size_t index) -> bool; /*strong*/
		auto counter() -> size_t; /*noexcept*/
		auto size() ->size_t; /*noexcept*/
	private:
		std::unique_ptr<bool[]> _array;
		size_t _size;
		size_t _counter;
};

bitset::bitset(size_t size) : _array(std::make_unique<bool[]>(size)), _size(size), _counter(0){}

auto bitset::test(size_t index) -> bool
{
	if (index>size()) throw std::out_of_range("error");
	else return _array[index];
}
		
auto bitset::set(size_t index) -> void
{
	if (index<= size())
		if (test(index) != true)
		{
			_array[index] = true;
			++_counter;
			//std::cout<<_counter << "counter"<< std::endl;
		}
	else throw std::out_of_range("error");
}

auto bitset::reset(size_t index) -> void
{
	if (index<=size())
		if (test(index)!=false)
		{
			_array[index]=false;
			--_counter;
		}
		else throw std::out_of_range("error");

}
			
auto bitset::size()->size_t
{
return _size;
}

auto bitset::counter()-> size_t
{
return _counter;
}

// ╔══╦╗─╔╗─╔══╦══╦══╦════╦══╦═══╗
// ║╔╗║║─║║─║╔╗║╔═╣╔╗╠═╗╔═╣╔╗║╔═╗║
// ║╚╝║║─║║─║║║║║─║╚╝║─║║─║║║║╚═╝║
// ║╔╗║║─║║─║║║║║─║╔╗║─║║─║║║║╔╗╔╝
// ║║║║╚═╣╚═╣╚╝║╚═╣║║║─║║─║╚╝║║║║
// ╚╝╚╩══╩══╩══╩══╩╝╚╝─╚╝─╚══╩╝╚╝

template<typename T>
class allocator
{
public:
	explicit allocator(size_t size = 0);
	auto swap(allocator& other)->void; /*noexcept*/
	allocator(allocator const &); /*strong*/
	//allocator(allocator const&) = delete;
	auto operator=(allocator const&)->allocator& = delete;
	auto resize() -> void ;/*strong*/
	auto construct(T* _array, T const &) -> void; /*strong*/
	auto destroy(T * _array) -> void; /*noexcept*/
	auto get()->T *; /*noexcept*/
	auto get() const -> T const *; /*noexcept*/
	auto count() const -> size_t; /*noexcept*/
	auto full() const -> bool; /*noexcept*/
	auto empty() const -> bool; /*noexcept*/
	~allocator();
private:
	T * _array;
	size_t _size;
	std::unique_ptr<bitset> _map;
	auto destroy(T * first, T * last) -> void; /*noexcept*/
};

//placement new
template <typename T>
auto allocator<T>::construct(T * ptr, T const & value)->void {
	
	if (ptr<_array || ptr>=_array + _size || _map->test(ptr - _array) == true) throw std::out_of_range("error");
	new(ptr) T(value);
	
	_map->set(ptr-_array);
}

//destroy obj
template <typename T>
auto allocator<T>::destroy(T * ptr) -> void
{
	if (ptr < _array || ptr >= _array + _size || _map->test(ptr - _array) == false) throw std::out_of_range("error");
	ptr->~T();
	_map->reset(ptr - _array);
}

template<typename T>/////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
allocator<T>::allocator(allocator const & tmp) :
	allocator<T>(tmp._size)
{
	for (size_t i = 0; i < _size; ++i) 
		construct(_array + i, tmp._array[i]);
}

//destroy from to
template <typename T>
auto allocator<T>::destroy(T * first, T * last) -> void
{
	for (; first != last; ++first) {
		destroy(&*first);
	}
}

//constructor allocator
template <typename T>
allocator<T>::allocator(size_t size) : _array(static_cast<T *>(size == 0 ? nullptr : operator new(size * sizeof(T)))),
	_size(size),
	_map(std::make_unique<bitset>(size))
{};

//destructor allocator
template <typename T>
allocator<T>::~allocator() {
	if (_map->counter() > 0) {
		destroy(_array, _array + _map->counter());
	}
	operator delete(_array);

};

//swap allocator
template <typename T>
auto allocator<T>::swap(allocator& other)->void 
{
	std::swap(_array, other._array);
	std::swap(_size, other._size);
	std::swap(_map, other._map);
};

template<typename T>
auto allocator<T>::resize() -> void
{
	size_t size = _size * 2 + (_size == 0);
	allocator<T> tmp(size);
	for (size_t i = 0; i < _size; ++i) {
	if (_map->test(i))
{		tmp.construct(tmp._array + i, _array[i]);}
	}
	this->swap(tmp);
	//_size = size;
}


template<typename T>
auto allocator<T>::get() -> T * {
	return _array;
}

template<typename T>
auto allocator<T>::get() const -> T const * {
	return _array;
}

template<typename T>
auto allocator<T>::count() const -> size_t {
	return _map->counter();
}

template<typename T>
auto allocator<T>::empty() const -> bool {
	return (_map->counter() == 0);
}

template<typename T>
auto allocator<T>::full() const -> bool {
	return (_map->counter() == _size);
}

// ╔══╦════╦══╦══╦╗╔══╗
// ║╔═╩═╗╔═╣╔╗║╔═╣║║╔═╝
// ║╚═╗─║║─║╚╝║║─║╚╝║
// ╚═╗║─║║─║╔╗║║─║╔╗║
// ╔═╝║─║║─║║║║╚═╣║║╚═╗
// ╚══╝─╚╝─╚╝╚╩══╩╝╚══╝

template <typename T>
class stack
{
private:
	allocator<T> allocate;
public:
	explicit 
	stack(size_t size = 0);/*noexcept*/
	//stack(stack const &); /*strong*/
	auto count() const ->size_t;/*noexcept*/
	auto push(T const &)->void;/*strong*/
	auto pop()->void;/*strong*/
	auto top() const->T const &;/*strong*/
	auto top() -> T&;/*strong*/
	//~stack(); 	/*noexcept*/
	auto operator=(const stack &)->stack&;/*strong*/
	stack(stack const & other) =default;
	auto empty() const ->bool;/*noexcept*/

};

//
// template <typename T>
// auto copy_new(size_t count_m, size_t array_size_m, const T * tmp)->T* {
// 	T *mass = new T[array_size_m];
// 	std::copy(tmp, tmp + count_m, mass);
// 	return mass;
// }

// template <typename T>
// stack<T>::stack()  {};

template <typename T>
stack<T>::stack(size_t size): allocate(size) {};

// template <typename T>
// stack<T>::stack(stack const &stck) : allocator<T>(stck._size)
// {
// 	for (size_t  i = 0; i < stck._count; i++)
// 	{
// 		construct(allocator<T>::_array + i, stck._array[i]);
// 	}
// 	allocator<T>::_count = stck._count;
// };

template <typename T>
auto stack<T>::count() const ->size_t {
	return allocate.count();
}

template <typename T>
auto stack<T>::push(T const &val)->void 
	{
	if (allocate.empty() == true || allocate.full() == true) 
		allocate.resize();
	allocate.construct(allocate.get() + allocate.count(), val);
}


//count--
template <typename T>
auto stack<T>::pop()->void {
	if (allocate.count() == 0) throw std::logic_error("Stack's empty");
	allocate.destroy(allocate.get() + (this->count()-1)); 
}

//el.remove();
template <typename T>
auto stack<T>::top() const-> const T& {
	if (allocate.count() == 0) throw std::logic_error("Stack's empty");
	return(*(allocate.get() + this->count() - 1));
}

// template <typename T>
// stack<T>::~stack() {
// destroy(allocator<T>::_array, allocator<T>::_array + allocator<T>::_count);};

template <typename T>
auto stack<T>::top()->T&
{
	if (allocate.count() == 0) throw std::logic_error("Stack's empty");
	return(*(allocate.get() + this->count() - 1));
}

template <typename T>
auto stack<T>::operator=(const stack &tmp)->stack& {
	if (this != &tmp) 
		 stack(tmp).allocate.swap(allocate);
	return *this;
}

//stack.isEmpty()
template<typename T> 
auto stack<T>::empty() const ->bool {
	return (allocate.count() == 0);
}
