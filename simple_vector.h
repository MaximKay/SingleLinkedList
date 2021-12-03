#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iterator>
#include "array_ptr.h"

using namespace std::string_literals;

class ReserveObj {
public:
	ReserveObj(size_t capacity_to_reserve) : capacity_to_reserve_(capacity_to_reserve) {}

	size_t GetCap() {
		return capacity_to_reserve_;
	}
private:
	size_t capacity_to_reserve_{};
};

auto Reserve(size_t capacity_to_reserve) {
	return ReserveObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
	using Iterator = Type*;
	using ConstIterator = const Type*;

	SimpleVector() noexcept = default;

	// Создаёт вектор из size элементов, инициализированных значением по умолчанию
	explicit SimpleVector(size_t size) : size_(size), capacity_(size), array_ptr_(size) {
	}

	// Создаёт вектор из size элементов, инициализированных значением value
	SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), array_ptr_(size) {
		std::fill(begin(), end(), value);
	}

	// Создаёт вектор из std::initializer_list
	SimpleVector(std::initializer_list<Type> init) :
		size_(init.size()), capacity_(init.size()), array_ptr_(init.size()) {
		std::copy(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), begin());
	}

	SimpleVector(const SimpleVector& other) {
		Type* new_arr_ptr = nullptr;
		try {
			new_arr_ptr = new Type[other.GetCapacity()]{ Type{} };
		}
		catch (...) {
			delete[] new_arr_ptr;
			throw;
		};
		ArrayPtr<Type> temp_array(new_arr_ptr);
		std::copy(other.begin(), other.end(), &(temp_array[0]));
		array_ptr_.swap(temp_array);
		capacity_ = other.GetCapacity();
		size_ = other.GetSize();
	}

	SimpleVector(SimpleVector&& other) {
		Type* new_arr_ptr = nullptr;
		try {
			new_arr_ptr = new Type[other.GetCapacity()]{ Type{} };
		}
		catch (...) {
			delete[] new_arr_ptr;
			throw;
		};
		std::copy(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), new_arr_ptr);
		ArrayPtr<Type> temp_array(new_arr_ptr);
		array_ptr_.swap(temp_array);
		other.capacity_ = std::exchange(capacity_, other.capacity_);
		other.size_ = std::exchange(size_, other.size_);
	}

	SimpleVector& operator=(const SimpleVector& other) {
		if (*this != other) {
			SimpleVector temp(other);
			swap(temp);
		};
		return *this;
	}

	SimpleVector& operator=(SimpleVector&& other) {
		if (*this != other) {
			SimpleVector temp(std::move(other));
			swap(temp);
		};
		return *this;
	}

	SimpleVector(ReserveObj obj) {
		Reserve(obj.GetCap());
	}

	// Возвращает количество элементов в массиве
	size_t GetSize() const noexcept {
		return size_;
	}

	// Возвращает вместимость массива
	size_t GetCapacity() const noexcept {
		return capacity_;
	}

	// Сообщает, пустой ли массив
	bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	// Возвращает ссылку на элемент с индексом index
	Type& operator[](size_t index) noexcept {
		assert(index < size_);
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	const Type& operator[](size_t index) const noexcept {
		assert(index < size_);
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	// Выбрасывает исключение std::out_of_range, если index >= size
	Type& At(size_t index) {
		if (index >= size_) {
			throw std::out_of_range(""s);
		};
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	// Выбрасывает исключение std::out_of_range, если index >= size
	const Type& At(size_t index) const {
		if (index >= size_) {
			throw std::out_of_range(""s);
		};
		return array_ptr_[index];
	}

	// Обнуляет размер массива, не изменяя его вместимость
	void Clear() noexcept {
		size_ = 0;
	}

	// Изменяет размер массива.
	// При увеличении размера новые элементы получают значение по умолчанию для типа Type
	void Resize(size_t new_size) {
		if (new_size <= size_) {
			size_ = new_size;
		}
		else {
			if (new_size < capacity_) {
				while (size_ != new_size) {
					PushBack(std::move(Type{}));
				};
			}
			else {
				Type* ptr_to_new_array = nullptr;
				try {
					ptr_to_new_array = new Type[new_size * 2]{ Type{} };
				}
				catch (...) {
					delete[] ptr_to_new_array;
					throw;
				};
				std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), ptr_to_new_array);
				ArrayPtr<Type> new_array(ptr_to_new_array);
				array_ptr_.swap(new_array);
				capacity_ = new_size * 2;
				size_ = new_size;
			};
		};
	}

	void SizeIncrement() {
		++size_;
	}

	// Добавляет элемент в конец вектора
	// При нехватке места увеличивает вдвое вместимость вектора
	void PushBack(const Type& item) {
		size_ < capacity_ ? SizeIncrement() : Resize(size_ + 1);
		array_ptr_[size_ - 1] = item;
	}

	void PushBack(Type&& item) {
		size_ < capacity_ ? SizeIncrement() : Resize(size_ + 1);
		array_ptr_[size_ - 1] = std::move(item);
	}

	// Вставляет значение value в позицию pos.
	// Возвращает итератор на вставленное значение
	// Если перед вставкой значения вектор был заполнен полностью,
	// вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
	Iterator Insert(ConstIterator pos, const Type& value) {
		if (pos == end()) {
			PushBack(value);
			return Iterator(end() - 1);
		};
		size_t index = std::distance(begin(), Iterator(pos));
		size_ < capacity_ ? SizeIncrement() : Resize(size_ + 1);
		std::copy_backward(std::make_move_iterator(&(array_ptr_[index])), std::make_move_iterator(end() - 1), end());
		array_ptr_[index] = value;
		return Iterator(&(array_ptr_[index]));
	}

	Iterator Insert(ConstIterator pos, Type&& value) {
		if (pos == end()) {
			PushBack(std::move(value));
			return Iterator(end() - 1);
		};
		size_t index = std::distance(begin(), Iterator(pos));
		size_ < capacity_ ? SizeIncrement() : Resize(size_ + 1);
		std::copy_backward(std::make_move_iterator(&(array_ptr_[index])) , std::make_move_iterator(end() - 1), end());
		array_ptr_[index] = std::move(value);
		return Iterator(&(array_ptr_[index]));
	}

	// "Удаляет" последний элемент вектора. Вектор не должен быть пустым
	void PopBack() noexcept {
		if (!IsEmpty()) {
			--size_;
		};
	}

	// Удаляет элемент вектора в указанной позиции
	Iterator Erase(ConstIterator pos) {
		if (pos == (end() - 1)) {
			PopBack();
			return Iterator(end());
		};
		std::copy(std::make_move_iterator(Iterator(std::next(pos))), std::make_move_iterator(end()), Iterator(pos));
		--size_;
		return Iterator(pos);
	}

	Iterator Erase(Iterator&& pos) {
		if (pos == (end() - 1)) {
			PopBack();
			return Iterator(end());
		};
		std::copy(std::make_move_iterator(std::next(pos)), std::make_move_iterator(end()), pos);
		--size_;
		return Iterator(pos);
	}

	void Reserve(size_t new_capacity) {
		if (capacity_ < new_capacity) {
			Type* ptr = nullptr;
			try {
				ptr = new Type[new_capacity]{ Type{} };
			}
			catch (...) {
				delete[] ptr;
				throw;
			};
			std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), ptr);
			ArrayPtr<Type> temp_arr(ptr);
			array_ptr_.swap(temp_arr);
			capacity_ = new_capacity;
		};
	}

	// Обменивает значение с другим вектором
	void swap(SimpleVector& other) noexcept {
		array_ptr_.swap(other.array_ptr_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}

	// Возвращает итератор на начало массива
	// Для пустого массива может быть равен (или не равен) nullptr
	Iterator begin() noexcept {
		return size_ == 0 ? Iterator{} : &(array_ptr_[0]);
	}

	// Возвращает итератор на элемент, следующий за последним
	// Для пустого массива может быть равен (или не равен) nullptr
	Iterator end() noexcept {
		return size_ == 0 ? Iterator{} : &(array_ptr_[size_]);
	}

	// Возвращает константный итератор на начало массива
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator begin() const noexcept {
		return size_ == 0 ? ConstIterator{} : &(array_ptr_[0]);
	}

	// Возвращает итератор на элемент, следующий за последним
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator end() const noexcept {
		return size_ == 0 ? ConstIterator{} : &(array_ptr_[size_]);
	}

	// Возвращает константный итератор на начало массива
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator cbegin() const noexcept {
		return size_ == 0 ? ConstIterator{} : &(array_ptr_[0]);
	}

	// Возвращает итератор на элемент, следующий за последним
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator cend() const noexcept {
		return size_ == 0 ? ConstIterator{} : &(array_ptr_[size_]);
	}

	friend class ReserveObj;
private:
	size_t size_{};
	size_t capacity_{};
	ArrayPtr<Type> array_ptr_{};
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin())
		&& lhs.GetSize() == rhs.GetSize();
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return lhs < rhs || lhs == rhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs < rhs);
}