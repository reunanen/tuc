struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "picotest/picotest.h"

int main(int, char **)
{
    return RUN_ALL_TESTS();
}