#include "gtest/gtest.h"

#include "../test/tests/events/EventQueueTests.h"
#include "../test/tests/graphics/AnimatorTests.h"
#include "../test/tests/graphics/SpriteTests.h"
#include "../test/tests/scene/ActorTests.h"
#include "../test/tests/scene/SceneTests.h"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}