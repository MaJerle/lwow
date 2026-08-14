#include <stdio.h>
#include "lwow/lwow.h"
#include "test.h"

extern int test_run(void);

int
main(void) {
    int ret = 0;
    printf("Application start\r\n");
    ret = test_run();
    printf("Done\r\n");

    return ret;
}
