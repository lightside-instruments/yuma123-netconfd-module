/*
    module ietf-hardware-state
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

/* module static variables */
static ncx_module_t *ietf_hardware_state_mod;
static obj_template_t* hardware_obj;

#define BUFSIZE 10*1024

/* Registered callback functions */

static status_t
    get_hardware(ses_cb_t *scb,
                     getcb_mode_t cbmode,
                     val_value_t *vir_val,
                     val_value_t *dst_val)
{
    status_t res;
    char* ptr;
    res = NO_ERR;

    /* /hardware */

    char *cmd = "hardware-state-get";

    char buf[BUFSIZE];
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        assert(0);
    }

    ptr = fgets(buf, BUFSIZE, fp);

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        ptr = NULL;
    }

    if(ptr==NULL || 0==strlen(buf)) {
        return ERR_NCX_SKIPPED;
    }

    printf("hardware-state-get: %s", buf);

    res = val_set_cplxval_obj(dst_val,
                              vir_val->obj,
                              buf);
    /* disable cache */
    vir_val->cachetime = 0;

    return res;
}

/* The 3 mandatory callback functions: y_ietf_hardware_state_init, y_ietf_hardware_state_init2, y_ietf_hardware_state_cleanup */

status_t
    y_ietf_hardware_state_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t *agt_profile;
    status_t res;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        "ietf-hardware-state",
        NULL,
        &agt_profile->agt_savedevQ,
        &ietf_hardware_state_mod);
    if (res != NO_ERR) {
        return res;
    }

    hardware_obj = ncx_find_object(
        ietf_hardware_state_mod,
        "hardware");
    if (hardware_obj == NULL) {
        return SET_ERROR(ERR_NCX_DEF_NOT_FOUND);
    }

    return res;
}

status_t y_ietf_hardware_state_init2(void)
{
    status_t res;
    cfg_template_t* runningcfg;
    val_value_t* hardware_val;

    res = NO_ERR;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    if (!runningcfg || !runningcfg->root) {
        return SET_ERROR(ERR_INTERNAL_VAL);
    }

    hardware_val = val_new_value();
    assert(hardware_val != NULL);

    val_init_virtual(hardware_val,
                     get_hardware,
                     hardware_obj);

    val_add_child(hardware_val, runningcfg->root);


    return res;
}

void y_ietf_hardware_state_cleanup (void)
{
}
