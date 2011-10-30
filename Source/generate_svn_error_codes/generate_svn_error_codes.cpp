#include <stdlib.h>
#include <stdio.h>

#include "svn_error_codes.h"

int main( int argc, char **argv )
{
    apr_size_t a_size;

    printf( "\n" );
    printf( "class svn_err:\n" );
#include "generate_svn_error_codes.hpp"
    return 0;
}
