#include <stdio.h>
#include <assert.h>

static FILE* gpiof[3];

int relay_actuator_io_init(void)
{
    int ret;
    FILE* f;
    gpiof[0]=NULL;
    gpiof[1]=NULL;
    gpiof[2]=NULL;

    f = fopen("/sys/class/gpio/export","w");
    assert(f);
    ret=fwrite("17\n", 3,1, f);
    ret=fwrite("27\n", 3,1, f);
    ret=fwrite("23\n", 3,1, f);
    fclose(f);
    f = fopen("/sys/class/gpio/gpio17/direction", "w");
    assert(f);
    ret=fwrite("out\n", 4,1, f);
    fclose(f);
    f = fopen("/sys/class/gpio/gpio27/direction", "w");
    assert(f);
    ret=fwrite("out\n", 4,1, f);
    fclose(f);
    f = fopen("/sys/class/gpio/gpio23/direction", "w");
    assert(f);
    ret=fwrite("out\n", 4,1, f);
    fclose(f);

    f = fopen("/sys/class/gpio/gpio17/direction", "w");
    assert(f);
    ret=fwrite("out\n", 4,1, f);
    fclose(f);
    f = fopen("/sys/class/gpio/gpio27/direction", "w");
    assert(f);
    ret=fwrite("out\n", 4,1, f);
    fclose(f);
    f = fopen("/sys/class/gpio/gpio23/direction", "w");
    assert(f);
    ret=fwrite("out\n", 4,1, f);
    fclose(f);

    gpiof[0] = fopen("/sys/class/gpio/gpio17/value", "w");
    assert(gpiof[0]!=NULL);
    gpiof[1] = fopen("/sys/class/gpio/gpio27/value", "w");
    assert(gpiof[1]!=NULL);
    gpiof[2] = fopen("/sys/class/gpio/gpio23/value", "w");
    assert(gpiof[2]!=NULL);

    return 0;
}

int relay_actuator_io_set(unsigned int bitmask)
{
    int i;
    int ret;

    for(i=0;i<3;i++) {
        if(bitmask & (1<<i)) {
            ret=fwrite("1\n",3,1, gpiof[i]);
        } else {
            ret=fwrite("0\n",3,1, gpiof[i]);
        }
        fflush(gpiof[i]);

        assert(ret==1);
    }
    return 0;
}
