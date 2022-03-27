#include <cstddef>
#include <memory>
#include <iostream>

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

private:
  void Swap(Vector& other) noexcept;

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
    if (size == mem.cap)
        Reserve((mem.cap == 0 ) ? 1 : 2 * mem.cap);

    new (mem.data + size) T(elem);
    ++size;
}

template <typename T>
void Vector<T>::PushBack(T&& elem)
{
    if (size == mem.cap)
        Reserve((mem.cap == 0 ) ? 1 : 2 * mem.cap);

    new (mem.data + size) T(std::move(elem));
    ++size;
}

template <typename T>
template <typename ... Args>
T& Vector<T>::EmplaceBack(Args&&... args)
{
    if (size == mem.cap)
        Reserve((mem.cap == 0 ) ? 1 : 2 * mem.cap);

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
