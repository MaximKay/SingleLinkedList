#pragma once

template <typename Type>
class ArrayPtr {
public:
	// �������������� ArrayPtr ������� ����������
	ArrayPtr() = default;

	// ������ � ���� ������ �� size ��������� ���� Type.
	// ���� size == 0, ���� raw_ptr_ ������ ���� ����� nullptr
	explicit ArrayPtr(size_t size) {
		size == 0 ? raw_ptr_ = nullptr : raw_ptr_ = new Type[size]{ Type{} };
	}

	// ����������� �� ������ ���������, ��������� ����� ������� � ���� ���� nullptr
	explicit ArrayPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {
	}

	// ��������� �����������
	ArrayPtr(const ArrayPtr&) = delete;

	~ArrayPtr() {
		delete[] raw_ptr_;
	}

	// ��������� ������������
	ArrayPtr& operator=(const ArrayPtr&) = delete;

	// ���������� ��������� �������� � ������, ���������� �������� ������ �������
	// ����� ������ ������ ��������� �� ������ ������ ����������
	[[nodiscard]] Type* Release() noexcept {
		Type* array_ptr = raw_ptr_;
		raw_ptr_ = nullptr;
		return array_ptr;
	}

	// ���������� ������ �� ������� ������� � �������� index
	Type& operator[](size_t index) noexcept {
		return raw_ptr_[index];
	}

	// ���������� ����������� ������ �� ������� ������� � �������� index
	const Type& operator[](size_t index) const noexcept {
		return raw_ptr_[index];
	}

	// ���������� true, ���� ��������� ���������, � false � ��������� ������
	explicit operator bool() const {
		return raw_ptr_ ? true : false;
	}

	// ���������� �������� ������ ���������, ��������� ����� ������ �������
	Type* Get() const noexcept {
		return raw_ptr_;
	}

	// ������������ ��������� ��������� �� ������ � �������� other
	void swap(ArrayPtr& other) noexcept {
		Type* temp_ptr = raw_ptr_;
		raw_ptr_ = other.raw_ptr_;
		other.raw_ptr_ = temp_ptr;
	}

private:
	Type* raw_ptr_ = nullptr;
};