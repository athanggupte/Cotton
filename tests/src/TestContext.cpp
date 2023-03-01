#include <gtest/gtest.h>
#include <impl/Context.h>

TEST(ContextSuite, GetSetContextLoop)
{
	volatile int count = 4;
	int accumulator = 0;
	std::string s;
	Context context {};

	get_context(&context);

	if (count > 0)
	{
		--count;
		++accumulator;
		s += std::to_string(accumulator);
		set_context(&context);
	}

	EXPECT_EQ(count, 0);
	EXPECT_EQ(accumulator, 4);
	EXPECT_EQ(s, "1234");
}
