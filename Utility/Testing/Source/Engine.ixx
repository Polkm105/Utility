export module Engine;
import <memory>;
import <concepts>;
import <ranges>;
import <algorithm>;
import <unordered_map>;
import <string_view>;
import <cstddef>;
import <cassert>;
import Delegate;
import TypeName;

export class Engine;

export template <>
struct std::hash<TypeSafeID<Delegate<>, std::size_t>>
{
  std::size_t operator()(const TypeSafeID<Delegate<>, std::size_t>& id) const noexcept
  {
    return std::hash<std::size_t>{}(id.Value());
  }
};

export template <typename T>
concept EngineSystem =
requires (const T& c_t, T& t, Engine* e)
{
  { c_t.TypeName() } -> std::convertible_to<std::string_view>;
  { t.SetEngine(e) };
};

struct IEngineSystem
{
  virtual void SetEngine(Engine* engine) = 0;
  virtual std::string_view TypeName() const = 0;
  virtual ~IEngineSystem() {};
};

template <EngineSystem T>
struct EngineSystemConcrete : public IEngineSystem
{
  template <typename ... Args>
  EngineSystemConcrete(Args&&... args) :
    base(std::forward<Args>(args)...)
  {}

  void SetEngine(Engine* e) override { base.SetEngine(e); }
  std::string_view TypeName() const override { return base.TypeName(); }
  T base;
};

export class Engine
{
public:
  template<EngineSystem T, typename ... Args>
  void Add(Args&&... args)
  {
    systems_.push_back(std::make_unique<EngineSystemConcrete<T>>(std::forward<Args>(args)...));
    name_to_index_.emplace(systems_.back()->TypeName(), systems_.size() - 1);
    systems_.back()->SetEngine(this);
  }

  template<typename T>
  T& Get() const
  {
    const std::string_view sys_name = TypeName<T>();
    const auto& iter = name_to_index_.find(sys_name);

    assert(iter != name_to_index_.end());
    auto* ptr = systems_[iter->second].get();
    assert(dynamic_cast<EngineSystemConcrete<T>*>(ptr));
    auto* concrete = static_cast<EngineSystemConcrete<T>*>(ptr);
    return concrete->base;
  }

  void Init()
  {
    OnInit.Invoke();
    is_running_ = true;
  }

  void Run()
  {
    if (!is_running_) { throw std::exception("Systems have not been initialized"); }

    while (is_running_)
    {
      OnUpdate.Invoke();
    }
  }  

private:
  Delegate<> OnInit{};
  Delegate<> OnUpdate{};
  std::unordered_map<std::string_view, std::size_t> name_to_index_{};
  std::vector<std::unique_ptr<IEngineSystem>> systems_{};
  bool is_running_{ false };
};