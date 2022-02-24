#include <Manager.h>
#include <Delegate.h>
#include <iostream>

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

static Transform* MakeTransform(Engine* engine)
{
	return new Transform(engine);
}

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


int main()
{
	DelegateTest();

	return 0;
}
