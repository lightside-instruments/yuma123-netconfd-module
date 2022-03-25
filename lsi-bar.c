/*
    module lsi-bar
 */

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>


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

#define BAR_MOD "lsi-bar"


/* module static variables */
static ncx_module_t *lsi_bar_mod;


static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *bar_cur_val=NULL;
    val_value_t *bar_new_val=NULL;
    val_value_t *enabled_cur_val=NULL;
    val_value_t *enabled_new_val=NULL;


    if(config_new_val) {
        bar_new_val = val_find_child(config_new_val,
                                       BAR_MOD,
                                       "bar");
        if(bar_new_val) {
            enabled_new_val = val_find_child(bar_new_val,
                                       BAR_MOD,
                                       "enabled");
        }
    }

    if(config_cur_val) {
        bar_cur_val = val_find_child(config_cur_val,
                                       BAR_MOD,
                                       "bar");
        if(bar_cur_val) {
            enabled_cur_val = val_find_child(bar_cur_val,
                                       BAR_MOD,
                                       "enabled");
        }
    }


    /* if /bar/enabled is created-modified-deleted */

    if(enabled_cur_val!=NULL && enabled_new_val==NULL && VAL_BOOL(enabled_cur_val)==TRUE) {
        system("lsi-bar-set 0");
    } else if(enabled_new_val!=NULL && enabled_cur_val==NULL && VAL_BOOL(enabled_new_val)==TRUE) {
        system("lsi-bar-set 1");
    } else if(enabled_new_val!=NULL && enabled_cur_val!=NULL && VAL_BOOL(enabled_new_val)!=VAL_BOOL(enabled_cur_val)) {
        if(VAL_BOOL(enabled_new_val)) {
            system("lsi-bar-set 1");
        } else {
            system("lsi-bar-set 0");
        }
    }
    return NO_ERR;
}

/* Registered callback functions */

static val_value_t* prev_root_val = NULL;

static status_t y_commit_complete(void)
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

    return NO_ERR;
}
/* The 3 mandatory callback functions: y_lsi_bar_init, y_lsi_bar_init2, y_lsi_bar_cleanup */

status_t
    y_lsi_bar_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t *agt_profile;
    status_t res;

    res = NO_ERR;
 
    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        "lsi-bar",
        NULL,
        &agt_profile->agt_savedevQ,
        &lsi_bar_mod);
    if (res != NO_ERR) {
        return res;
    }


    res=agt_commit_complete_register("lsi-bar" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    return res;
}

status_t y_lsi_bar_init2(void)
{
    /* emulate initial commit */
    y_commit_complete();

    return NO_ERR;
}

void y_lsi_bar_cleanup (void)
{
}
