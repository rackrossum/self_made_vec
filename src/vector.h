#include <algorithm>
#include <cstddef>
#include <memory>
#include <iostream>
#include <algorithm>

template <typename T>
struct RawMemory
{
    T* data;
    size_t cap;

    RawMemory();
    RawMemory(size_t n);
    ~RawMemory();

    RawMemory& operator = (RawMemory&& other);

    static T* Allocate(size_t n);
    static void Deallocate(T* data);

    void Swap(RawMemory<T>& other) noexcept;
};

template <typename T>
RawMemory<T>::RawMemory()
    :data(nullptr), cap(0)
{}

template <typename T>
RawMemory<T>::RawMemory(size_t n)
    : data(Allocate(n)), cap(n)
{}

template <typename T>
RawMemory<T>::~RawMemory()
{
    Deallocate(data);
}

template <typename T>
T* RawMemory<T>::Allocate(size_t n)
{
    return static_cast<T*>(operator new(n * sizeof(T)));
}

template <typename T>
void RawMemory<T>::Deallocate(T* data)
{
    operator delete (data);
}

template <typename T>
void RawMemory<T>::Swap(RawMemory<T>& other) noexcept
{
    std::swap(data, other.data);
    std::swap(cap, other.cap);
}

//Vector
template <typename T>
class Vector {
public:
  Vector() = default;
  Vector(size_t n);
  Vector(const Vector& other);
  Vector(Vector&& other);

  ~Vector();

  Vector& operator = (const Vector& other);
  Vector& operator = (Vector&& other) noexcept;

  void Reserve(size_t n);

  void Resize(size_t n);

  void PushBack(const T& elem);
  void PushBack(T&& elem);

  template <typename ... Args>
  T& EmplaceBack(Args&&... args);

  void PopBack();

  size_t Size() const noexcept;

  size_t Capacity() const noexcept;

  const T& operator[](size_t i) const;
  T& operator[](size_t i);

  // В данной части задачи реализуйте дополнительно эти функции:
  using iterator = T*;
  using const_iterator = const T*;

  iterator begin() noexcept;
  iterator end() noexcept;

  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  // Тут должна быть такая же реализация, как и для константных версий begin/end
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

  // Вставляет элемент перед pos
  // Возвращает итератор на вставленный элемент
  iterator Insert(const_iterator pos, const T& elem);
  iterator Insert(const_iterator pos, T&& elem);

  // Конструирует элемент по заданным аргументам конструктора перед pos
  // Возвращает итератор на вставленный элемент
  template <typename ... Args>
  iterator Emplace(const_iterator it, Args&&... args);

  // Удаляет элемент на позиции pos
  // Возвращает итератор на элемент, следующий за удалённым
  iterator Erase(const_iterator it);

private:
  void Swap(Vector& other) noexcept;

  void Realloc();

  iterator InsertEmpty(const_iterator it);

private:
  RawMemory<T> mem;
  size_t size = 0;
};

template <typename T>
Vector<T>::Vector(size_t n)
    :mem(n), size(n)
{
    std::uninitialized_value_construct_n(mem.data, n);
}

template <typename T>
Vector<T>::Vector(const Vector& other)
    :mem(other.size), size(other.size)
{
    std::uninitialized_copy_n(other.mem.data, other.size, mem.data);
}

template <typename T>
Vector<T>::Vector(Vector&& other)
{
    Swap(other);
}

template <typename T>
Vector<T>::~Vector()
{
    std::destroy_n(mem.data, size);
}

template <typename T>
Vector<T>& Vector<T>::operator = (const Vector& other)
{
    if (other.size > mem.cap)
    {
        Vector temp(other);
        Swap(temp);
    }
    else
    {
        for (auto i = 0; i < size && i < other.size; ++i)
            mem.data[i] = other.mem.data[i];

        if (size > other.size)
            std::destroy_n(mem.data + other.size, size - other.size);
        else if (size < other.size)
            std::uninitialized_copy_n(other.mem.data + size, other.size - size, mem.data + size);
        size = other.size;
    }

    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator = (Vector&& other) noexcept
{
    Swap(other);
    return *this;
}

template <typename T>
void Vector<T>::Reserve(size_t n)
{
    if (mem.cap < n)
    {
        RawMemory<T> temp(n);
        std::uninitialized_move_n(mem.data, size, temp.data);
        std::destroy_n(mem.data, size);
        mem.Swap(temp);
    }
}

template <typename T>
void Vector<T>::Resize(size_t n)
{
    if (n > mem.cap)
        Reserve(n);

    if (n > size)
        std::uninitialized_value_construct_n(mem.data + size, n - size);
    else if (n < size)
        std::destroy_n(mem.data + n, size - n);

    size = n;
}

template <typename T>
void Vector<T>::PushBack(const T& elem)
{
    Realloc();
    new (mem.data + size) T(elem);
    ++size;
}

template <typename T>
void Vector<T>::PushBack(T&& elem)
{
    Realloc();
    new (mem.data + size) T(std::move(elem));
    ++size;
}

template <typename T>
template <typename ... Args>
T& Vector<T>::EmplaceBack(Args&&... args)
{
    Realloc();
    auto elem = new (mem.data + size) T(std::forward<Args>(args)...);
    ++size;

    return *elem;
}


template <typename T>
void Vector<T>::PopBack()
{
    std::destroy_at(mem.data + size - 1);
    --size;
}

template <typename T>
size_t Vector<T>::Size() const noexcept
{
    return size;
}

template <typename T>
size_t Vector<T>::Capacity() const noexcept
{
    return mem.cap;
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{
    return *(mem.data + i);
}

template <typename T>
const T& Vector<T>::operator[](size_t i) const
{
    return *(mem.data + i);
}
template <typename T>
void Vector<T>::Swap(Vector<T>& other) noexcept
{
    mem.Swap(other.mem);
    std::swap(size, other.size);
}

template <typename T>
void Vector<T>::Realloc()
{
    if (size == mem.cap)
        Reserve((mem.cap == 0 ) ? 1 : 2 * mem.cap);
}

//part 2
template <typename T>
typename Vector<T>::iterator Vector<T>::begin() noexcept
{
    return mem.data;
}

template <typename T>
typename Vector<T>::iterator Vector<T>::end() noexcept
{
    return mem.data + size;
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const noexcept
{
    return mem.data;
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::end() const noexcept
{
    return mem.data + size;
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::cbegin() const noexcept
{
    return begin();
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::cend() const noexcept
{
    return end();
}

template <typename T>
typename Vector<T>::iterator Vector<T>::Insert(typename Vector<T>::const_iterator pos, const T& elem)
{
    auto dest = InsertEmpty(pos);
    return new (dest) T(elem);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::Insert(typename Vector<T>::const_iterator pos, T&& elem)
{
    auto dest = InsertEmpty(pos);
    return new (dest) T(std::move(elem));
}

template <typename T>
template <typename ... Args>
typename Vector<T>::iterator Vector<T>::Emplace(typename Vector<T>::const_iterator it, Args&&... args)
{
    auto dest = InsertEmpty(it);
    return new (dest) T(std::forward<Args>(args)...);
}

template <typename T>
typename Vector<T>::iterator Vector<T>::InsertEmpty(typename Vector<T>::const_iterator pos)
{
    auto d = pos - begin();
    Realloc();
    for (auto it = end(); it != begin() + d; --it)
    {
        std::swap(*(it), *(it - 1));  
    }
    //std::move_backward(begin() + d, end(), end() + 1);
    ++size;
    return begin() + d;
}

template <typename T>
typename Vector<T>::iterator Vector<T>::Erase(typename Vector<T>::const_iterator pos)
{
    auto d = pos - begin();
    std::destroy_at(pos);
    std::move(begin() + d + 1, end(), begin() + d);
    --size;
    return begin() + d;
}

