#include "unity.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../../examples/autotest-validate/autotest-validate.h"
#include "../../assignment-autotest/test/assignment1/username-from-conf-file.h"

//Attributions: I used the link 'https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md as a reference for the code
// Additionally, I brushed up on pointers and used it here



/**
* This function should:
*   1) Call the my_username() function in autotest-validate.c to get your hard coded username.
*   2) Obtain the value returned from function malloc_username_from_conf_file() in username-from-conf-file.h within
*       the assignment autotest submodule at assignment-autotest/test/assignment1/
*   3) Use unity assertion TEST_ASSERT_EQUAL_STRING_MESSAGE to verify the two strings are equal.  See
*       the [unity assertion reference](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md)
*/
void test_validate_my_username()
{


/* This will obtain hardcoded username string from autotest-validate.c */
    const char* hardcoded_user = my_username();

    /* Read username from /conf/username.txt and allocate memory for the string */
    char* config_user = malloc_username_from_conf_file();

    /*Will compare strings and trigger test failure if they are not the same */
    TEST_ASSERT_EQUAL_STRING_MESSAGE(
        hardcoded_user, 
        config_user, 
        "Username mismatch between hardcoded value and config file"
    );

    /* Deallocate memory previously assigned to config_user */
    free(config_user);

}
