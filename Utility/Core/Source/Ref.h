#pragma once
#include <concepts>
#include <functional>
#include "ID.h"

template <typename T, typename IDType = ID<T>>
concept MemoryVolatile = requires(T a, std::function<void(T*)> func, IDType id)
{
  { RegisterOnMove(a, func) } -> std::convertible_to<ID<T>>;
  { UnregisterOnMove(a, id) };
  std::movable<T>;
};

template<MemoryVolatile T>
class Ref
{
public:
  Ref(T& ref) :
    base_(&ref),
    id_(RegisterWithBase())
  {}

  Ref(T* ptr) :
    base_(ptr),
    id_(RegisterWithBase())
  {
  }

  Ref(Ref& rhs) :
    base_(rhs.base_),
    id_(RegisterWithBase())
  {
  }

  Ref& operator=(Ref& rhs)
  {
    UnregisterWithBase();
    base_ = rhs.base_;
    id_ = RegisterWithBase();
  }

  ~Ref()
  {
    UnregisterWithBase();
  }

  operator T& () { return *base_; }
  operator const T& () const { return *base_; }

  bool IsValid() { return base_ != nullptr; }
  T* Raw() const { return base_; }

  T* operator->() { return base_; }

private:
  ID<T> RegisterWithBase()
  {
    return RegisterOnMove(*base_, [this](T* ptr) { OnReallocate(ptr); });
  }

  void UnregisterWithBase()
  {
    if (IsValid())
    {
      UnregisterOnMove(*base_, id_);
    }
  }
  void OnReallocate(T* ptr)
  {
    base_ = ptr;
  }

  T* base_{ nullptr };
  ID<T> id_{};
};
