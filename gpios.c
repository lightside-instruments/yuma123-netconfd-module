/*
    module lsi-gpios
    namespace urn:lsi:params:xml:ns:yang:gpios
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

#define GPIOS_MOD "lsi-gpios"

static obj_template_t* gpios_state_obj;


static status_t
    get_gpios_state(ses_cb_t *scb,
                         getcb_mode_t cbmode,
                         val_value_t *vir_val,
                         val_value_t *dst_val)
{
    status_t res;
    char* ptr;
    res = NO_ERR;

    /* /gpios-state */

    char *cmd = "gpios-get";

    char buf[BUFSIZE]="";
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        assert(0);
    }
    do {
        ptr = fgets(buf+strlen(buf), BUFSIZE, fp);
    } while(ptr);

    printf("gpios-get: %s", buf);

    assert(strlen(buf));

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        assert(0);
    }

    res = val_set_cplxval_obj(dst_val,
                              vir_val->obj,
                              buf);
    /* disable cache */
    vir_val->cachetime = 0;

    return res;
}


static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *gpios_val;
    val_value_t *gpio_val;
    val_value_t *name_val;
    val_value_t *level_val;
    unsigned int i;

    char setcmd_buf[1024]="gpios-set "; // append parameters e.g. "gpios-set gpio27=1 gpio5=0"

    if(config_new_val == NULL) {
        gpios_val = NULL;
    } else {
        gpios_val = val_find_child(config_new_val,
                               GPIOS_MOD,
                               "gpios");
    }

    if(gpios_val!=NULL) {
        for (gpio_val = val_get_first_child(gpios_val);
             gpio_val != NULL;
             gpio_val = val_get_next_child(gpio_val)) {
            name_val = val_find_child(gpio_val,
                               GPIOS_MOD,
                               "name");

            level_val = val_find_child(gpio_val,
                                      GPIOS_MOD,
                                      "level");

            if(level_val) {
                sprintf(setcmd_buf+strlen(setcmd_buf), " %s=%d",VAL_STRING(name_val), VAL_UINT32(level_val));
            } else {
                sprintf(setcmd_buf+strlen(setcmd_buf), " %s",VAL_STRING(name_val));
            }
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

/* The 3 mandatory callback functions: y_lsi_gpios_init, y_lsi_gpios_init2, y_lsi_gpios_cleanup */

status_t
    y_lsi_gpios_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        GPIOS_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }
    res=agt_commit_complete_register("lsi-gpios" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    gpios_state_obj = ncx_find_object(
        mod,
        "gpios-state");
    if (gpios_state_obj == NULL) {
        return SET_ERROR(ERR_NCX_DEF_NOT_FOUND);
    }

    system("gpios-init");

    return res;
}

status_t y_lsi_gpios_init2(void)
{
    status_t res=NO_ERR;
    cfg_template_t* runningcfg;
    val_value_t* gpios_state_val;

    res = NO_ERR;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg && runningcfg->root);

    gpios_state_val = val_new_value();
    assert(gpios_state_val != NULL);

    val_init_virtual(gpios_state_val,
                     get_gpios_state,
                     gpios_state_obj);

    val_add_child(gpios_state_val, runningcfg->root);

    /* emulate initial startup configuration commit */
    y_commit_complete();

    return res;
}

void y_lsi_gpios_cleanup (void)
{
}
