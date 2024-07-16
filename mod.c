/*
    module lsi-gcode
    namespace urn:lsi:params:xml:ns:yang:gcode
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

#define GCODE_MOD "lsi-gcode"

static obj_template_t* gcode_state_obj;


static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *gcode_cur_val;
    val_value_t *gcode_new_val;
    val_value_t *command_cur_val;
    val_value_t *command_new_val;
    unsigned int i;
    unsigned int deleted;

    char setcmd_buf[1024]="gcode-run --command=123 "; // append parameters e.g. "gcode-run --command=123"

    if(config_new_val == NULL) {
        gcode_new_val = NULL;
    } else {
        gcode_new_val = val_find_child(config_new_val,
                               GCODE_MOD,
                               "gcode");
    }

    if(config_cur_val == NULL) {
        gcode_cur_val = NULL;
    } else {
        gcode_cur_val = val_find_child(config_cur_val,
                               GCODE_MOD,
                               "gcode");
    }

    if(gcode_cur_val!=NULL) {

        command_cur_val = val_find_child(gcode_cur_val,
                           GCODE_MOD,
                           "command");

        //if this light instance is deleted turn off
        if(gcode_new_val!=NULL) {
            command_new_val = val_find_child(gcode_cur_val,
                               GCODE_MOD,
                               "command");
            if(0==strcmp(VAL_STRING(command_new_val),VAL_STRING(command_cur_val))) {
                deleted=0;
            }

//            if(deleted) {
//            	//do nothing, no effect
//            }
        }
    }


    if(gcode_new_val!=NULL) {
        command_new_val = val_find_child(gcode_new_val,
                           GCODE_MOD,
                           "command");
        sprintf(setcmd_buf, "gcode-run --command=\"%s\"", VAL_STRING(command_new_val));
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

/* The 3 mandatory callback functions: y_lsi_gcode_init, y_lsi_gcode_init2, y_lsi_gcode_cleanup */

status_t
    y_lsi_gcode_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        GCODE_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }
    res=agt_commit_complete_register("lsi-gcode" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    system("gcode-init");

    return res;
}

status_t y_lsi_gcode_init2(void)
{
    status_t res=NO_ERR;

    /* emulate initial startup configuration commit */
    y_commit_complete();

    return res;
}

void y_lsi_gcode_cleanup (void)
{
}
