#include "macros.h"
#include "test_commons.h"

#include <stdio.h>

/**
 * @brief Runs a specified test function multiple times and prints the result.
 *
 * This function takes a test function pointer, the name of the function, and the number of times
 * to run the test. It executes the test function the specified number of times and then prints
 * a message indicating that the test passed.
 *
 * @param test_fun A pointer to the test function to be executed.
 * @param fn_name A string representing the name of the test function.
 * @param n The number of times to run the test function.
 *
 * @note If the macro PARAMS is defined, the output message will include the value of PARAMS.
 */
void run_test(void (*test_fun)(), const char *fn_name, int n) {
    for (int i = 0; i < n; ++i) {
        test_fun();
    }

    #ifdef PARAMS
    printf("[%s]: %s: OK\n", xstr(PARAMS), fn_name);
    #else
    printf("%s: OK\n", fn_name);
    #endif
}