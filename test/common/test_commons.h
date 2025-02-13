#ifndef TEST_COMMONS_H
#define TEST_COMMONS_H

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
void run_test(void (*)(), const char *, int);

#endif // TEST_COMMONS_H
