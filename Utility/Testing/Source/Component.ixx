export module Component;
import <string_view>;

export struct Component
{
public:
  virtual void Init() {};
  virtual void Update(float dt) { dt; };
  virtual const std::string_view TypeName() const = 0;
  virtual ~Component() {};
};