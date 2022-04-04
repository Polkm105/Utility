
#define ENGINE 1
#define COMPONENTS 1
#define FLYWEIGHT 0
#define MANAGERS 1
#define DELEGATES 0
#define	REFERENCES 1


#include <iostream>


#if ENGINE

class Engine
{
};

#endif

#if COMPONENTS
class Component
{

};
#include <functional>
#include <ID.h>
class Transform : public Component
{
public:
  Transform(Engine* engine) noexcept:
    engine_(engine),
    on_move_functions_(),
    id_counter_(0)
  {}
  Transform(Transform&& other) noexcept:
    engine_(other.engine_),
    on_move_functions_(std::move(other.on_move_functions_)),
    id_counter_(other.id_counter_)
  {
    for (auto& pair : on_move_functions_)
    {
      pair.second(this);
    }
  }

  Transform& operator=(Transform&& other) noexcept
  {
    engine_ = other.engine_;
    on_move_functions_ = std::move(other.on_move_functions_);
    id_counter_ = other.id_counter_;

    for (auto& pair : on_move_functions_)
    {
      pair.second(this);
    }

    return *this;
  }

  void Init() {}
  void Update(float dt) { dt; }

  friend void Init(Transform& transform)
  {
    transform.Init();
  }

  friend void Update(Transform& transform, float dt)
  {
    transform.Update(dt);
  }

  friend ID<Transform> RegisterOnMove(Transform& transform, std::function<void(Transform*)> func)
  {
    ID<Transform> id(++transform.id_counter_);
    transform.on_move_functions_.push_back(std::make_pair(id, func));
    return id;
  }

  friend void UnregisterOnMove(Transform& transform, const ID<Transform>& id_to_remove)
  {
    size_t i = 0;
    auto& funcs = transform.on_move_functions_;
    for (const auto& [id, func] : transform.on_move_functions_)
    {
      if (id == id_to_remove)
      {
        std::swap(funcs.back(), funcs[i]);
        funcs.pop_back();
        break;
      }

      ++i;
    }
  }

private:
  Engine* engine_;
  std::vector<std::pair<ID<Transform>, std::function<void(Transform*)>>> on_move_functions_;
  std::size_t id_counter_;
};

static Transform* MakeTransform(Engine* engine)
{
  return new Transform(engine);
}
#else
class Engine;
static Transform* MakeTransform(Engine* engine) { return nullptr; }
#endif

#if DELEGATES
#include <Delegate.h>

void VoidVoid() { std::cout << "Void Void\n"; }
static void DelegateTest()
{
  using DelegateType = Delegate<Engine&>;
  Engine eng;
  DelegateType del;

  const size_t numFuncs = 10;
  DelegateType::IDType delIDs[numFuncs];
  size_t i;

  for (i = 0; i < numFuncs; ++i)
  {
    delIDs[i] = del.Register([i](Engine& eng) { std::cout << "Test" << i << "\n"; });
  }

  std::cout << "Invoke Test\n";
  del.Invoke(eng);

  for (i = 0; i < numFuncs; i += 2)
  {
    del.UnRegister(delIDs[i]);
    delIDs[i] = DelegateType::IDType(0);
  }

  std::cout << "Invoke Test\n";
  del.Invoke(eng);

  for (i = 0; i < numFuncs; ++i)
  {
    if (del.Has(delIDs[i]))
    {
      del.UnRegister(delIDs[i]);
    }
  }

  std::cout << "Invoke Test\n";
  del.Invoke(eng);
}
#else
static void DelegateTest() {}
#endif

#if FLYWEIGHT
#include <BasicFlyweight.h>

static void FlyweightTest()
{
  Engine eng;
  BasicFlyweight fly;

  fly.Add<Transform>(&eng);
  fly.Remove(0);

  fly.Init();
  fly.Update(.01f);
}
#else
static void FlyweightTest() {}
#endif

#if MANAGERS
#include <Manager.h>
void TestManagers()
{
  Engine eng;
  Manager<Transform> transform_man;

  transform_man.Create(&eng);
}
#else
void TestManagers() {}
#endif

#if REFERENCES
#include <Ref.h>
static void TestReferences()
{
  Engine e;
  ManagedVector<Transform> trans_vec;

  auto id = trans_vec.Add(Transform(&e), ID<Transform>(1));
  auto id2 = trans_vec.Add(Transform(&e), ID<Transform>(2));

  Ref<Transform> trans_ref(trans_vec.Get(id2));
  trans_vec.Remove(id);

  trans_ref->Init();
  trans_ref->Update(.01f);
  
}
#else
static void TestReferences() {}
#endif

int main()
{
  FlyweightTest();
  TestReferences();
  TestManagers();

  return 0;
}
