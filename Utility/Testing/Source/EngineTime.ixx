export module EngineTime;

import Engine;
import <chrono>;
import <string_view>;

export class EngineTime
{
public:
  void Init() {}
  void Update() {}

  void SetEngine(Engine* e) { engine_ = e; }
  std::string_view TypeName() const { return "EngineTime"; }

private:
  Engine* engine_;
};