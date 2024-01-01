/*
    module lsi-rover
    namespace urn:lsi:params:xml:ns:yang:rover
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

#define ROVER_MOD "lsi-rover"

static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *rover_new_val;
    val_value_t *rover_cur_val;
    val_value_t *angle_new_val;
    val_value_t *angle_cur_val;
    val_value_t *distance_new_val;
    val_value_t *distance_cur_val;
    val_value_t *velocity_new_val;
    val_value_t *velocity_cur_val;

    unsigned int i;

    char setcmd_buf[]="lsi-rover-rotate 180";
    char* ptr;

    if(config_new_val) {
        rover_new_val = val_find_child(config_new_val,
                               ROVER_MOD,
                               "rover");
    }
    if(config_cur_val) {
        rover_cur_val = val_find_child(config_cur_val,
                               ROVER_MOD,
                               "rover");
    }

    if(rover_new_val) {
        angle_new_val = val_find_child(rover_new_val,      
                               ROVER_MOD,
                               "angle");
        distance_new_val = val_find_child(rover_new_val,      
                               ROVER_MOD,
                               "distance");
        velocity_new_val = val_find_child(rover_new_val,      
                               ROVER_MOD,
                               "velocity");
    }
    if(rover_cur_val) {
        angle_cur_val = val_find_child(rover_cur_val,      
                               ROVER_MOD,
                               "angle");
        distance_cur_val = val_find_child(rover_cur_val,      
                               ROVER_MOD,
                               "distance");
        velocity_cur_val = val_find_child(rover_cur_val,      
                               ROVER_MOD,
                               "velocity");
    }

    if((rover_cur_val == NULL) && rover_new_val) {
        system("lsi-rover-activate &");
    } else if((rover_new_val == NULL) && rover_cur_val) {
        system("lsi-rover-dock &");
    } else {
    	sprintf(setcmd_buf, "lsi-rover-move %d %d %d &",VAL_INT32(angle_new_val)-VAL_INT32(angle_cur_val), VAL_INT32(distance_new_val)-VAL_INT32(distance_cur_val), VAL_INT32(velocity_cur_val));
        system(setcmd_buf);
    }

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

/* The 3 mandatory callback functions: y_lsi_rover_init, y_lsi_rover_init2, y_lsi_rover_cleanup */

status_t
    y_lsi_rover_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        ROVER_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }
    res=agt_commit_complete_register("lsi-rover" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    return res;
}

status_t y_lsi_rover_init2(void)
{
    status_t res=NO_ERR;
    cfg_template_t* runningcfg;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg && runningcfg->root);

    y_commit_complete();

    return res;
}

void y_lsi_rover_cleanup (void)
{
}
