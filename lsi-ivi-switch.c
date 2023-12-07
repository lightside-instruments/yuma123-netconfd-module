/*
    module lsi-ivi-switch
    namespace urn:lsi:params:xml:ns:yang:ivi-switch
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <libxml/xmlstring.h>
#include "procdefs.h"
#include "agt.h"
#include "agt_cb.h"
#include "agt_commit_complete.h"
#include "agt_timer.h"
#include "agt_util.h"
#include "agt_not.h"
#include "agt_rpc.h"
#include "dlq.h"
#include "ncx.h"
#include "ncxmod.h"
#include "ncxtypes.h"
#include "status.h"
#include "rpc.h"
#include "val.h"
#include "val123.h"
#include <sys/time.h>
#include "relay-actuator-io.h"
#define SWITCH_MOD "lsi-ivi-switch"

static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *channels_val;
    val_value_t *channel_val;
    val_value_t *name_val;
    val_value_t *connections_val;
    val_value_t *connection_val;

    unsigned int i;
    int ret;
    unsigned int bitmask = 0;
    unsigned int index = 0;


    if(config_new_val == NULL) {
        channels_val = NULL;
    } else {
        channels_val = val_find_child(config_new_val,
                               SWITCH_MOD,
                               "channels");
    }


    if(channels_val!=NULL) {
        for (channel_val = val_get_first_child(channels_val);
             channel_val != NULL;
             channel_val = val_get_next_child(channel_val)) {
            name_val = val_find_child(channel_val,
                               SWITCH_MOD,
                               "name");

            connections_val = val_find_child(channel_val,      
                               SWITCH_MOD,
                               "connections");
            if(connections_val!=NULL) {
                for (connection_val = val_get_first_child(connections_val);
                     connection_val != NULL;
                     connection_val = val_get_next_child(connection_val)) {

                    index=VAL_STRING(connection_val)[1]-'1';
                    assert(index<6);
                    if(VAL_STRING(name_val)[0]=='a' || VAL_STRING(connection_val)[0]=='a')
                    bitmask = bitmask | (1<<index);
                }
            }
        }
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    ret=relay_actuator_io_set(bitmask);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken;
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("call time=%lf", time_taken);
    
    assert(ret==0);

    return NO_ERR;
}


static val_value_t* prev_root_val = NULL;
static int update_config_wrapper()
{
    cfg_template_t        *runningcfg;
    status_t res;
    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg!=NULL && runningcfg->root!=NULL);
    if(prev_root_val!=NULL) {
        val_value_t* cur_root_val;
        cur_root_val = val_clone_config_data(runningcfg->root, &res);
        if(0==val_compare(cur_root_val,prev_root_val)) {
            /*no change*/
            val_free_value(cur_root_val);
            return 0;
        }
        val_free_value(cur_root_val);
    }
    update_config(prev_root_val, runningcfg->root);

    if(prev_root_val!=NULL) {
        val_free_value(prev_root_val);
    }
    prev_root_val = val_clone_config_data(runningcfg->root, &res);

    return 0;
}

static status_t y_commit_complete(void)
{
    update_config_wrapper();
    return NO_ERR;
}

/* The 3 mandatory callback functions: y_lsi_ivi_switch_init, y_lsi_ivi_switch_init2, y_lsi_ivi_switch_cleanup */

status_t
    y_lsi_ivi_switch_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    int ret;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        SWITCH_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }
    res=agt_commit_complete_register("lsi-ivi-switch" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    ret=relay_actuator_io_init();
    assert(ret==0);

    return res;
}

status_t y_lsi_ivi_switch_init2(void)
{
    status_t res=NO_ERR;
    cfg_template_t* runningcfg;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg && runningcfg->root);

    y_commit_complete();

    return res;
}

void y_lsi_ivi_switch_cleanup (void)
{
}
