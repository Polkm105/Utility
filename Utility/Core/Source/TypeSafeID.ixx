//---------------------------------------------------------------------------------------
// Author:      Kenny Mecham
// Date:        02/24/2022
// Project:     Utility
// Description: A type safe ID system used in managers and other systems that hold data
//              for specific types.
//---------------------------------------------------------------------------------------
export module TypeSafeID;
import <cstddef>;
import <type_traits>;
import <concepts>;

export template <typename T>
concept Incrementable = requires(T t) { ++t; };
// A type safe ID system
// binds to the type that it is an ID for, and the underlying type to use for it's ID
export template <typename Type, Incrementable StorageType = std::size_t>
class TypeSafeID
{
public:
  using InternalType = StorageType;

  explicit TypeSafeID(StorageType id = 0) noexcept :
    id_(id)
  {}

  TypeSafeID(const TypeSafeID&) = default;
  TypeSafeID& operator=(const TypeSafeID&) = default;
  TypeSafeID(TypeSafeID&&) = default;
  TypeSafeID& operator=(TypeSafeID&&) = default;

  bool operator==(const TypeSafeID& other) const noexcept
  {
    return Value() == other.Value();
  }

  StorageType Value() const noexcept
  {
    return id_;
  }

  bool IsValid() const noexcept
  {
    return id_ != 0;
  }

private:
  StorageType id_{ 0 };
};

export template<typename T, Incrementable U>
struct std::hash<TypeSafeID<T, U>>
{
  std::size_t operator()(const TypeSafeID<T, U>& id) const noexcept
  {
    return std::hash<U>{}(id.Value());
  }
};
