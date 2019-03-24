struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "picotest/picotest.h"

int main(int argc, char **argv)
{
    RUN_ALL_TESTS();
}