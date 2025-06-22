#include "led/ledFader.h"

TEST_CASE( "Test ledFader", "[ledFader]" ) {

    ledFader_t data;
    ledFader_initialize(&data);

    SECTION( "No Fade" ) {
        ledFader_fade(&data, 10, 0, false);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 10);
    }

    SECTION( "Fade Up" ) {
        ledFader_fade(&data, 10, 5, false);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 2);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 4);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 6);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 8);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 10);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 10);
    }

    SECTION( "Fade Down" ) {
        data.currentValue = 10;
        ledFader_fade(&data, 0, 5, false);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 8);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 6);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 4);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 2);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 0);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 0);
    }

    SECTION( "Fade Up 2" ) {
        ledFader_fade(&data, 10, 3, false);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 3);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 6);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 9);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 10);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 10);
    }

    SECTION( "Fade Down 2" ) {
        data.currentValue = 10;
        ledFader_fade(&data, 0, 3, false);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 7);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 4);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 1);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 0);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 0);
    }

    SECTION( "Fade Up 3" ) {
        ledFader_fade(&data, 5, 10, false);

        // this is wrong
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 1);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 2);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 3);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 4);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 5);
        ledFader_handler(&data);
        REQUIRE(data.currentValue == 5);
    }
}
