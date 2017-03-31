#ifndef LED_PATTERN_H
#define LED_PATTERN_H

#include <stdint.h>

namespace APP {
  
class LedInterval {
public:
    uint16_t m_levelPermil;     // Brightness level 0-1000.
    uint16_t m_durationMs;      // Duration in millisecond.
    
    uint16_t GetLevelPermil() const { return m_levelPermil; }
    uint16_t GetDurationMs() const { return m_durationMs; }
};

class LedPattern {
public:
    enum {
        COUNT = 256
    };
    uint32_t m_count;                // Number of intervals in use.
    LedInterval m_interval[COUNT];   // Array of intervals. Used ones start from index 0.
    
    // Must perform range check. Assert if invalid.
    //uint32_t GetCount() const { ... }
    //LedInterval const &GetInterval(uint32_t index) { ... }
};

class LedPatternSet {
public:
    enum {
        COUNT = 4
    };
    uint32_t m_count;               // Number of patterns in use.
    LedPattern m_pattern[COUNT];    // Array of patterns. Used ones start from index 0.
    
    // Must perform range check. Assert if invalid.
    //uint32_t GetCount() const { ... }
    //LedPattern const &GetPattern(uint32_t index) { ... }
};

extern LedPatternSet const TEST_LED_PATTERN_SET;

} // namespace APP

#endif // LED_PATTERN_H