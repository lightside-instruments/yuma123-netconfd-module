/*
    module lsi-ivi-function-generator
    namespace urn:lsi:params:xml:ns:yang:ivi-function-generator
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

#define FUNCTION_GENERATOR_MOD "lsi-ivi-function-generator"
static char* visa_resource_name;
static obj_template_t* outputs_state_obj;

int run_arbitrary_waveform(val_value_t* name_val, val_value_t* arbitrary_waveform_val)
{

    val_value_t *data_val;
    FILE* f;
    char buf[BUFSIZE];
    char* ptr;

    data_val = val_find_child(arbitrary_waveform_val,
                                      FUNCTION_GENERATOR_MOD,
                                      "data");
    assert(data_val);
    sprintf(buf, "/tmp/%s-signal.wav", VAL_STRING(name_val));
    f=fopen(buf, "w");
    assert(f);
    fwrite(data_val->v.binary.ustr, data_val->v.binary.ustrlen, 1, f);
    fflush(f);
    fclose(f);
    sprintf(buf, "aplay -D \"%s\"  \"/tmp/%s-signal.wav\" &", VAL_STRING(name_val), VAL_STRING(name_val));

    printf("Calling: %s\n", buf);
    system(buf);

    return NO_ERR;
}

int run_standard_function(val_value_t* name_val, val_value_t* standard_function_val)
{
    val_value_t *waveform_type_val;
    val_value_t *frequency_val;
    val_value_t *amplitude_val;
    val_value_t *dc_offset_val;
    val_value_t *duty_cycle_val;
    char buf[BUFSIZE];
    char* ptr;

    waveform_type_val = val_find_child(standard_function_val,
                                      FUNCTION_GENERATOR_MOD,
                                      "waveform-type");
    assert(waveform_type_val);

    amplitude_val = val_find_child(standard_function_val,
                                      FUNCTION_GENERATOR_MOD,
                                      "amplitude");

    dc_offset_val = val_find_child(standard_function_val,
                                      FUNCTION_GENERATOR_MOD,
                                      "dc-offset");

    frequency_val = val_find_child(standard_function_val,
                                      FUNCTION_GENERATOR_MOD,
                                      "frequency");

    duty_cycle_val = val_find_child(standard_function_val,
                                      FUNCTION_GENERATOR_MOD,
                                      "duty-cycle");

    if(0==strcmp("default",VAL_STRING(name_val))) {
        char* frequency_str;
        char* amplitude_str;
        char* dc_offset_str;
        char* duty_cycle_str;

        if(frequency_val) {
            frequency_str = val_make_sprintf_string(frequency_val);
        }
        if(amplitude_val) {
            amplitude_str = val_make_sprintf_string(amplitude_val);
        }

        if(dc_offset_val) {
            dc_offset_str = val_make_sprintf_string(dc_offset_val);
        }

        if(0==strcmp(VAL_STRING(waveform_type_val),"square")) {
            if(duty_cycle_val) {
                duty_cycle_str = val_make_sprintf_string(duty_cycle_val);
    	    }
            sprintf(buf, "lsi-ivi-function-generator-set on square %s %s %s %s", frequency_str, amplitude_str, dc_offset_str, duty_cycle_val?duty_cycle_str:"50");
            if(duty_cycle_val) {
                free(duty_cycle_str);
            }
        } else if(0==strcmp(VAL_STRING(waveform_type_val),"sine")) {
            assert(frequency_val);
            sprintf(buf, "lsi-ivi-function-generator-set on sine %s %s %s", frequency_str, amplitude_str, dc_offset_val?dc_offset_str:"0");
        } else if(0==strcmp(VAL_STRING(waveform_type_val),"dc")) {
            sprintf(buf, "lsi-ivi-function-generator-set on dc %s %s %s", frequency_val?frequency_str:0, amplitude_val?amplitude_str:0, dc_offset_val?dc_offset_str:"0");
        }

        if(frequency_val) {
            free(frequency_str);
        }

        free(amplitude_str);

        if(dc_offset_val) {
            free(dc_offset_str);
        }

        printf("Calling: %s\n", buf);
        system(buf);
    }

    return NO_ERR;
}

static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *channels_val;
    val_value_t *channel_val;
    val_value_t *name_val;
    val_value_t *arbitrary_waveform_val;
    val_value_t *data_val=NULL;
    val_value_t *standard_function_val;

    unsigned int i;

    if(config_new_val == NULL) {
        channels_val = NULL;
    } else {
        channels_val = val_find_child(config_new_val,
                               FUNCTION_GENERATOR_MOD,
                               "channels");
    }

    if(channels_val!=NULL) {
        for (channel_val = val_get_first_child(channels_val);
             channel_val != NULL;
             channel_val = val_get_next_child(channel_val)) {
            name_val = val_find_child(channel_val,
                               FUNCTION_GENERATOR_MOD,
                               "name");

            standard_function_val = val_find_child(channel_val,
                               FUNCTION_GENERATOR_MOD,
                               "standard-function");

            if(standard_function_val!=NULL) {
                return run_standard_function(name_val, standard_function_val);
            }

            arbitrary_waveform_val = val_find_child(channel_val,
                               FUNCTION_GENERATOR_MOD,
                               "arbitrary-waveform");

            if(arbitrary_waveform_val!=NULL) {
                return run_arbitrary_waveform(name_val, arbitrary_waveform_val);
            }
            
        }
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

/* The 3 mandatory callback functions: y_lsi_ivi_function_generator_init, y_lsi_ivi_function_generator_init2, y_lsi_ivi_function_generator_cleanup */

status_t
    y_lsi_ivi_function_generator_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        FUNCTION_GENERATOR_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }

    res=agt_commit_complete_register("lsi-ivi-function-generator" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    return res;
}

status_t y_lsi_ivi_function_generator_init2(void)
{
    status_t res=NO_ERR;
    cfg_template_t* runningcfg;

    res = NO_ERR;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg && runningcfg->root);

    /* emulate initial startup configuration commit */
    y_commit_complete();

    return res;
}

void y_lsi_ivi_function_generator_cleanup (void)
{
}
