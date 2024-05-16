#include "../common/tickTimer.h"

TEST_CASE( "Test tickTimer", "[tickTimer]" ) {

    SECTION("Test basic functionality") {
        tickTimer_init();

        tickTimer_t timer;
        tickTimer_reset(&timer);
        REQUIRE(tickTimer_delay5ms(&timer,1) == false);
        tickTimer_interruptHandler();
        REQUIRE(tickTimer_delay5ms(&timer,1) == true);
    }

    SECTION("Test more functionality") {
        tickTimer_init();

        tickTimer_t timer;
        tickTimer_reset(&timer);
        REQUIRE(tickTimer_delay5ms(&timer,1) == false);
        tickTimer_interruptHandler();
        REQUIRE(tickTimer_delay5ms(&timer,1) == true);
        REQUIRE(tickTimer_delay5ms(&timer,1) == false);
        REQUIRE(tickTimer_delay5ms(&timer,2) == false);
        tickTimer_interruptHandler();
        REQUIRE(tickTimer_delay5ms(&timer,2) == false);
        tickTimer_interruptHandler();
        REQUIRE(tickTimer_delay5ms(&timer,1) == true);
    }

    SECTION("Test timer reset") {
        tickTimer_init();

        tickTimer_t timer;
        tickTimer_reset(&timer);
        REQUIRE(tickTimer_delay5ms(&timer,1) == false);
        tickTimer_interruptHandler();
        tickTimer_reset(&timer);
        REQUIRE(tickTimer_delay5ms(&timer,1) == false);
        tickTimer_interruptHandler();
        REQUIRE(tickTimer_delay5ms(&timer,1) == true);
    }

    SECTION("Test overflow") {
        tickTimer_init();

        for(uint32_t i= 0; i<0xFFFE;i++)
        {
            tickTimer_interruptHandler();
        }

        tickTimer_t timer;
        tickTimer_reset(&timer);
        REQUIRE(tickTimer_delay5ms(&timer,0x02) == false);
        tickTimer_interruptHandler();
        tickTimer_interruptHandler();
        REQUIRE(tickTimer_delay5ms(&timer,0x02) == true);
    }
}