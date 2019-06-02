#include "gtest/gtest.h"

#include "main/milkTests.h"
#include "math/Matrix3Tests.h"
#include "math/RectangleTests.h"
#include "node/NodeTreeTests.h"

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}