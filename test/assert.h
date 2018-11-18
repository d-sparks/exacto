#ifndef exacto_test_assert_h
#define exacto_test_assert_h

#include <iostream>
#include <string>

#define ASSERT(condition, message)                                             \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__         \
                << " line "  << __LINE__ << ": " << message << std::endl;      \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (false)

#define ASSERT_EQ(exp1, exp2, message)                                         \
  do {                                                                         \
    if (!((exp1) == (exp2))) {                                                 \
      std::cerr << "Assertion `" #exp1 " == " #exp2 "` failed in " << __FILE__ \
           << " line "  << __LINE__ << ": " << message << std::endl;           \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (false)

#endif  // exacto_test_assert_h
