#include <interface.h>
#include <stdio.h>
#include "arg_parser.h"

int main(int argc, char **argv)
{
    if (!parse_args(argv, argc))
    {
        printf("PekarDrive: %s\n", error_msg());
        return 1;
    }

    return 0;
}
