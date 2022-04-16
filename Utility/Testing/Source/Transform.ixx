export module Transform;
import Component;

export class Transform : public Component
{
public:


private:

  const std::string_view TypeName() const override { return "Transform"; }
  //Delegate<Transform*> on_move_;
};
