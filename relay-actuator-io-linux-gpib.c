#include <stdio.h>
#include <assert.h>
#include <gpib/ib.h>

#define GPIB_PAD 3
static int device_descriptor;

int relay_actuator_io_init(void)
{
    int ret;
    device_descriptor=ibdev(0,GPIB_PAD,0,T1000s/*T3s*/,0,0);
    assert(device_descriptor>=0);

    return 0;
}

int relay_actuator_io_set(unsigned int bitmask)
{
    int i;
    int ret;
    char setcmd_buf[]="B135A246";
    char* ptr;
    /* e.g. B135A246 */
    if(bitmask != 0x3F) {
        ptr = setcmd_buf;
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

    ibwrt(device_descriptor, setcmd_buf,8);
    return 0;
}
