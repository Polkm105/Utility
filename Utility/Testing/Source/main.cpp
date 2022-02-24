#include <Manager.h>
#include <Delegate.h>

class Engine
{
};

class Component
{

};

class Transform : public Component
{
public:
	Transform(Engine* engine) :
		engine_(engine) {}
	Transform(Transform&& other) = default;
	Transform& operator=(Transform&& other) = default;
	void Update(float dt) { dt; }
	void Init() {}

private:
	Engine* engine_;
};

Transform* MakeTransform(Engine* engine)
{
	return new Transform(engine);
}

void VoidVoid() {}

const int numIDs = 10;
ID<Transform> transIDs[numIDs];

int main()
{
	Engine* eng = new Engine();
	Manager<Transform> tMan;
	Delegate<> test;

	auto val = test.Register(VoidVoid);
	test.UnRegister(val);
	test.Invoke();

	int i;
	for (i = 0; i < numIDs; ++i)
	{
		transIDs[i] = tMan.Create(eng);
	}

	tMan.Update(1);

	for (i = 1; i < numIDs; i *= 2)
	{
		tMan.Remove(transIDs[i]);
		transIDs[i] = ID<Transform>();
	}
	tMan.Update(1);

	for (i = 0; i < numIDs; i += 2)
	{
		if (tMan.Has(transIDs[i]))
		{
			tMan.Remove(transIDs[i]);
			transIDs[i] = ID<Transform>();
		}
	}
	tMan.Update(1);

	for (i = 1; i < numIDs; i *= 2)
	{
		if (!transIDs[i].IsValid())
		{
			transIDs[i] = tMan.Create(eng);
		}
	}
	tMan.Update(1);

	for (i = 0; i < numIDs; i += 2)
	{
		if (!transIDs[i].IsValid())
		{
			transIDs[i] = tMan.Create(eng);
		}
	}
	tMan.Update(1);

	delete eng;
	return 0;
}
