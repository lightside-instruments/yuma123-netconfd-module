/*
    module lsi-lights
    namespace urn:lsi:params:xml:ns:yang:lights
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
#include "val_set_cplxval_obj.h"


#define BUFSIZE 1000000

#define LIGHTS_MOD "lsi-lights"

static obj_template_t* lights_state_obj;


static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *lights_cur_val;
    val_value_t *lights_new_val;
    val_value_t *light_val;
    val_value_t *name_val;
    val_value_t *red_val;
    val_value_t *green_val;
    val_value_t *blue_val;
    unsigned int i;

    char setcmd_buf[1024]="lights-set "; // append parameters e.g. "lights-set left-red=FF0000 center-green=00FF00 right-blue=0000FF"

    if(config_new_val == NULL) {
        lights_new_val = NULL;
    } else {
        lights_new_val = val_find_child(config_new_val,
                               LIGHTS_MOD,
                               "lights");
    }

    if(config_cur_val == NULL) {
        lights_cur_val = NULL;
    } else {
        lights_cur_val = val_find_child(config_cur_val,
                               LIGHTS_MOD,
                               "lights");
    }

    if(lights_cur_val!=NULL) {
        for (light_val = val_get_first_child(lights_cur_val);
             light_val != NULL;
             light_val = val_get_next_child(light_val)) {
            int deleted;
            deleted = 1;

            name_val = val_find_child(light_val,
                               LIGHTS_MOD,
                               "name");

            //if this light instance is deleted turn off
            if(lights_new_val!=NULL) {
                val_value_t * light_new_val;
                val_value_t * name_new_val;
                for (light_new_val = val_get_first_child(lights_new_val);
                     light_new_val != NULL;
                     light_new_val = val_get_next_child(light_new_val)) {
                    name_new_val = val_find_child(light_new_val,
                                       LIGHTS_MOD,
                                       "name");
                    if(0==strcmp(VAL_STRING(name_new_val),VAL_STRING(name_val))) {
                        deleted=0;
                    }
                }
            }
            if(deleted) {
                sprintf(setcmd_buf+strlen(setcmd_buf), " %s=%02X%02X%02X", VAL_STRING(name_val), 0, 0, 0);
            }
        }
    }


    if(lights_new_val!=NULL) {
        for (light_val = val_get_first_child(lights_new_val);
             light_val != NULL;
             light_val = val_get_next_child(light_val)) {
            name_val = val_find_child(light_val,
                               LIGHTS_MOD,
                               "name");

            red_val = val_find_child(light_val,
                                      LIGHTS_MOD,
                                      "red");
            green_val = val_find_child(light_val,
                                      LIGHTS_MOD,
                                      "green");
            blue_val = val_find_child(light_val,
                                      LIGHTS_MOD,
                                      "blue");

            sprintf(setcmd_buf+strlen(setcmd_buf), " %s=%02X%02X%02X", VAL_STRING(name_val), VAL_UINT8(red_val), VAL_UINT8(green_val), VAL_UINT8(blue_val));
        }
    }


    printf("Calling: %s\n", setcmd_buf);
    system(setcmd_buf);

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

/* The 3 mandatory callback functions: y_lsi_lights_init, y_lsi_lights_init2, y_lsi_lights_cleanup */

status_t
    y_lsi_lights_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        LIGHTS_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }
    res=agt_commit_complete_register("lsi-lights" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    system("lights-init");

    return res;
}

status_t y_lsi_lights_init2(void)
{
    status_t res=NO_ERR;

    /* emulate initial startup configuration commit */
    y_commit_complete();

    return res;
}

void y_lsi_lights_cleanup (void)
{
}
