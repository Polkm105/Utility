import TypeSafeID;
import Transform;
import Engine;
import EngineTime;

import <string_view>;

struct TestEngineSystem
{
  void Init() {}
  void Update() {}
  inline void SetEngine(Engine* e) { engine_ = e; }
  std::string_view TypeName() const { return "TestEngineSystem"; }

  Engine* engine_;
};

int main()
{
  Engine engine;

  engine.Add<TestEngineSystem>();
  engine.Add<EngineTime>();

  engine.Init();
  engine.Run();

  return 0;
}
