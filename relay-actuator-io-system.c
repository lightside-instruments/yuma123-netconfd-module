#include <stdlib.h>

int relay_actuator_io_init(void)
{
    return 0;
}

int relay_actuator_io_set(unsigned int bitmask)
{
    int i;
    int ret;
    char setcmd_buf[]="lsi-ivi-switch-set B135A246";
    char* ptr;
    /* e.g. B135A246 */
    if(bitmask != 0x3F) {
        ptr = setcmd_buf + strlen("lsi-ivi-switch-set ");
        *ptr++='B';
        for(i=0;i<6;i++) {
            if(bitmask & (1<<i)) {
                continue;
            }
            *ptr++=i+'1';
        }
    }
    if(bitmask != 0) {
        *ptr++='A';
        for(i=0;i<6;i++) {
            if(bitmask & (1<<i)) {
                *ptr++=i+'1';
            }
        }
    }
    *ptr=0;

    ret = system(setcmd_buf);
    return ret;
}
