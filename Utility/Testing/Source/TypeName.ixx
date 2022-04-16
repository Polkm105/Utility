export module TypeName;
import <string_view>;

export template<typename T>
std::string_view TypeName()
{
  return T{}.TypeName();
}