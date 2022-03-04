#include <gtest/gtest.h>

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(SanityCheck, ZeroCheck)
{
	ASSERT_EQ(0, false);
	ASSERT_EQ(0, 1 ^ 1);
}
