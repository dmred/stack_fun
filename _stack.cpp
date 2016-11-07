// stack.cpp: îïðåäåëÿåò òî÷êó âõîäà äëÿ êîíñîëüíîãî ïðèëîæåíèÿ.
//


#include <iostream>

using namespace std;
using std::size_t;


template <typename T>
class stack
{
public:
	stack();
	stack(stack const & stck);
	auto count() const noexcept ->size_t;
	auto push(T const & value)->void;
	auto top() const->const T&;
	auto pop()->T;
	auto operator=(stack const & stck)->stack &;
	~stack();
private:
	T * _array;
	size_t _array_size;
	size_t _count;
};

template <typename T>
stack<T>::stack() : _array(nullptr), _array_size(0), _count(0) {};

template <typename T>
stack<T>::~stack() {
	delete[] _array;
}

template <typename T>
stack<T>::stack(stack const &stck) : _array_size(stck._array_size), count_(stck._count) {
	_array = new T[_array_size];
	for (int i = 0; i < _count; i++) _array[i] = stck._array[i];
}

template<typename T>
 auto stack<T>::operator=(stack const & stck) -> stack & {
	if (this != &stck) {
		(stack(stck)).swap(*this);
	}
	return *this;
}

template <typename T>
auto stack<T>::count() const noexcept->size_t { return _count; }

template<typename T>
 auto stack<T>::push(T const & value)->void {
	if (_count == _array_size)
	{
		_array_size *= 2;
		T * time = new T[array_size_];
		copy(_array, _array_size + _array, time
		);
		delete[] _array;

		array_ = time;
		delete[] time;
	}
	_array[_count] = value;
	_count++;

}

template <typename T>
auto stack<T>::top() const -> T& {
	if (_count == 0) throw logic_error("stack's empty");
	return _array[_count];
}


template <typename T>
auto stack<T>::pop() -> T {
	if (_count == 0) throw("stack's empty");
	return --_count;
}
