#include <gtest/gtest.h>
#include "impl/Fiber_old.h"

TEST(FiberSuite, SwitchingBetweenFibers)
{
	std::unique_ptr<Cotton::Fiber_old> main, fiber1, fiber2, fiber3;
	std::string s;
	fiber1 = std::make_unique<Cotton::Fiber_old>([&]()
	{
		s += "1";
		fiber1->SwitchTo(fiber2.get());
		s += "4";
		fiber1->SwitchTo(fiber2.get());
	}, Cotton::DEFAULT_STACK_SIZE);
	fiber1->id = 1;
	fiber2 = std::make_unique<Cotton::Fiber_old>([&]()
	{
		s += "2";
		fiber2->SwitchTo(fiber3.get());
		s += "5";
		fiber2->SwitchTo(fiber3.get());
	}, Cotton::DEFAULT_STACK_SIZE);
	fiber2->id = 2;
	fiber3 = std::make_unique<Cotton::Fiber_old>([&]()
	{
		s += "3";
		fiber3->SwitchTo(fiber1.get());
		s += "6";
		fiber3->SwitchTo(main.get());
	}, Cotton::DEFAULT_STACK_SIZE);
	fiber3->id = 3;
	main = std::make_unique<Cotton::Fiber_old>();
	main->SwitchTo(fiber1.get());

	EXPECT_EQ(s, "123456");
}

TEST(FiberSuite, CoroutineYield)
{
	std::string s;
	std::unique_ptr<Cotton::Fiber_old> fiber1 = std::make_unique<Cotton::Fiber_old>([&]()
	{
		for (int i = 0; i < 6; i++)
		{
			s += std::to_string(i+1);
			fiber1->Yield();
		}
	}, Cotton::DEFAULT_STACK_SIZE);

	std::unique_ptr<Cotton::Fiber_old>main = std::make_unique<Cotton::Fiber_old>();
	int counter = 0;

	while (fiber1->IsAlive()) {
		main->SwitchTo(fiber1.get());
		counter++;
	}

	EXPECT_EQ(counter, 7); // iterated 7 times : 6 times inside `for` loop + 1 time for returning/finishing
	EXPECT_EQ(s, "123456");
}
