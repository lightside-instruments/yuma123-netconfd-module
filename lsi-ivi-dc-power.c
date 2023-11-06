/*
    module lsi-ivi-dc-power
    namespace urn:lsi:params:xml:ns:yang:ivi-dc-power
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

#define DC_POWER_MOD "lsi-ivi-dc-power"
static char* visa_resource_name;
static obj_template_t* outputs_state_obj;


static status_t
    get_outputs_state(ses_cb_t *scb,
                         getcb_mode_t cbmode,
                         val_value_t *vir_val,
                         val_value_t *dst_val)
{
    status_t res;
    char* ptr;
    res = NO_ERR;

    /* /outputs-state */

    char cmd_buf[BUFSIZE]="lsi-ivi-dc-power-get";
    char buf[BUFSIZE]="";
    FILE *fp;

    sprintf(cmd_buf+strlen(cmd_buf), " %s", visa_resource_name);
    if ((fp = popen(cmd_buf, "r")) == NULL) {
        printf("Error opening pipe!\n");
        assert(0);
    }
    do {
        ptr = fgets(buf+strlen(buf), BUFSIZE, fp);
    } while(ptr);

    printf("lsi-ivi-dc-power-get: %s", buf);

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

    val_value_t *outputs_val;
    val_value_t *output_val;
    val_value_t *name_val;
    val_value_t *voltage_level_val=NULL;
    val_value_t *current_limit_val=NULL;
    val_value_t *voltage_level1_val=NULL;
    val_value_t *current_limit1_val=NULL;
    val_value_t *voltage_level2_val=NULL;
    val_value_t *current_limit2_val=NULL;

    unsigned int i;

    char setcmd_buf[BUFSIZE];
    char* ptr;

    if(config_new_val == NULL) {
        outputs_val = NULL;
    } else {
        outputs_val = val_find_child(config_new_val,
                               DC_POWER_MOD,
                               "outputs");
    }

    if(outputs_val!=NULL) {
        for (output_val = val_get_first_child(outputs_val);
             output_val != NULL;
             output_val = val_get_next_child(output_val)) {
            name_val = val_find_child(output_val,
                               DC_POWER_MOD,
                               "name");

            current_limit_val = val_find_child(output_val,
                                      DC_POWER_MOD,
                                      "current-limit");

            voltage_level_val = val_find_child(output_val,
                                      DC_POWER_MOD,
                                      "voltage-level");

            if(0==strcmp(VAL_STRING(name_val), "out1")) {
                current_limit1_val = current_limit_val;
                voltage_level1_val = voltage_level_val;
            } else if(0==strcmp(VAL_STRING(name_val), "out2")) {
                current_limit2_val = current_limit_val;
                voltage_level2_val = voltage_level_val;
            } 
        }
    }


    strcpy(setcmd_buf, "lsi-ivi-dc-power-set");
    sprintf(setcmd_buf+strlen(setcmd_buf), " \"%s\"", visa_resource_name);
    if(current_limit1_val) {
        char* current_limit_str;
        char* voltage_level_str;
        current_limit_str = val_make_sprintf_string(current_limit1_val);
        voltage_level_str = val_make_sprintf_string(voltage_level1_val);
        sprintf(setcmd_buf+strlen(setcmd_buf), " %s %s %s", "on", voltage_level_str, current_limit_str);
        free(current_limit_str);
        free(voltage_level_str);
    } else {
        sprintf(setcmd_buf+strlen(setcmd_buf), " off 0 0");
    }

    if(current_limit2_val) {
        char* current_limit_str;
        char* voltage_level_str;
        current_limit_str = val_make_sprintf_string(current_limit2_val);
        voltage_level_str = val_make_sprintf_string(voltage_level2_val);
        sprintf(setcmd_buf+strlen(setcmd_buf), " %s %s %s", "on", voltage_level_str, current_limit_str);
        free(current_limit_str);
        free(voltage_level_str);
    } else {
        sprintf(setcmd_buf+strlen(setcmd_buf), " off 0 0");
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

/* The 3 mandatory callback functions: y_lsi_ivi_dc_power_init, y_lsi_ivi_dc_power_init2, y_lsi_ivi_dc_power_cleanup */

status_t
    y_lsi_ivi_dc_power_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        DC_POWER_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }

    visa_resource_name = getenv ("LSI_IVI_DC_POWER_VISA_RESOURCE_NAME");
    if(visa_resource_name==NULL) {
        fprintf(stderr, "Environment variable LSI_IVI_DC_POWER_VISA_RESOURCE_NAME must be defined. E.g. setenv LSI_IVI_DC_POWER_VISA_RESOURCE_NAME=\"TCPIP::192.168.14.20::gpib,2::INSTR\"");
        return SET_ERROR(ERR_INTERNAL_VAL);
    } else {
        printf("LSI_IVI_DC_POWER_VISA_RESOURCE_NAME=%s",visa_resource_name);
    }


    res=agt_commit_complete_register("lsi-ivi-dc-power" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    outputs_state_obj = ncx_find_object(
        mod,
        "outputs-state");
    if (outputs_state_obj == NULL) {
        return SET_ERROR(ERR_NCX_DEF_NOT_FOUND);
    }

    return res;
}

status_t y_lsi_ivi_dc_power_init2(void)
{
    status_t res=NO_ERR;
    cfg_template_t* runningcfg;
    val_value_t* outputs_state_val;

    res = NO_ERR;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg && runningcfg->root);

    outputs_state_val = val_new_value();
    assert(outputs_state_val != NULL);

    val_init_virtual(outputs_state_val,
                     get_outputs_state,
                     outputs_state_obj);

    val_add_child(outputs_state_val, runningcfg->root);

    /* emulate initial startup configuration commit */
    y_commit_complete();

    return res;
}

void y_lsi_ivi_dc_power_cleanup (void)
{
}
