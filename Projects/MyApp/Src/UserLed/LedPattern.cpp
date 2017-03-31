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
    2, {
        // Pattern 0
        {2, {
            {1,99}, {2, 3}
        }},
        // Pattern 1
        {3, {
            {1,76}, {2, 3}, {3, 4}
        }}
    }
};

} // namespace APP