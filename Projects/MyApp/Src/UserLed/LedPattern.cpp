#include "LedPattern.h"

namespace APP {

// Illustration only.
/*
LedInterval testInterval = {1, 2};

LedPattern testPattern = {
    2, {{1,2}, {2, 3}}
};
*/

LedPatternSet const TEST_LED_PATTERN_SET = {
    3, {
        // Pattern 0
        {5, {
            {1,99}, {2, 3}, {100, 50}, {2, 3}, {100, 50}
        }},
        // Pattern 1
        {5, {
            {1,76}, {2, 3}, {3, 4}, {50, 85}, {99, 70}
        }},
        // Pattern 2
        {5, {
            {5,6}, {2, 3}, {75, 43}, {5, 84}, {29, 40}
        }},
    }
};

} // namespace APP