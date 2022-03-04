#include <gtest/gtest.h>
#include "FiberImpl.h"

TEST(FiberImplSuite, FiberImpl)
{
	std::unique_ptr<Cotton::FiberImpl> main, fiber1, fiber2, fiber3;
	std::string s;
	fiber1 = std::make_unique<Cotton::FiberImpl>([&]()
	{
		s += "1";
		fiber1->SwitchTo(fiber2.get());
	}, Cotton::DEFAULT_STACK_SIZE);
	fiber1->id = 1;
	fiber2 = std::make_unique<Cotton::FiberImpl>([&]()
	{
		s += "2";
		fiber2->SwitchTo(fiber3.get());
	}, Cotton::DEFAULT_STACK_SIZE);
	fiber2->id = 2;
	fiber3 = std::make_unique<Cotton::FiberImpl>([&]()
	{
		s += "3";
		fiber3->SwitchTo(main.get());
	}, Cotton::DEFAULT_STACK_SIZE);
	fiber3->id = 3;
	main = std::make_unique<Cotton::FiberImpl>();
	main->SwitchTo(fiber1.get());

	EXPECT_EQ(s, "123");
}
