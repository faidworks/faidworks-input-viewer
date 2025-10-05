#include "controller.h"
#include <gtest/gtest.h>

class ControllerTest : public ::testing::Test
{
protected:
    Controller controller;

    void SetUp() override
    {
        controller.setUp();
    }

    void TearDown() override
    {
        controller.tearDown();
    }
};

TEST_F(ControllerTest, UpdateState)
{
    controller.update();
    Controller::GamepadState state = controller.getState();
    EXPECT_TRUE(state.buttons >= 0); // Ensure the state button value is valid
}
