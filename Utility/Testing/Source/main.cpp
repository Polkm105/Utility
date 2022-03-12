
#define ENGINE
#define COMPONENTS
#define FLYWEIGHT


#include <iostream>


#ifdef ENGINE

class Engine
{
};

#endif

#ifdef COMPONENTS
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

private:
	Engine* engine_;
};

static Transform* MakeTransform(Engine* engine)
{
	return new Transform(engine);
}

#endif


#ifdef DELEGATES
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
#endif

#ifdef FLYWEIGHT
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
#endif

int main()
{
	FlyweightTest();

	return 0;
}
