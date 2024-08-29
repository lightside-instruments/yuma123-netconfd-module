/*
    module lsi-ivi-scope
    namespace urn:lsi:params:xml:ns:yang:ivi-scope
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <libxml/xmlstring.h>
#include <inttypes.h>
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


#define BUFSIZE 4000000

#define SCOPE_MOD "lsi-ivi-scope"
static obj_template_t* data_obj;

static status_t
    get_data(ses_cb_t *scb,
                         getcb_mode_t cbmode,
                         val_value_t *vir_val,
                         val_value_t *dst_val)
{
    status_t res;
    char* ptr;
    val_value_t* name_val;
    res = NO_ERR;


    char buf[BUFSIZE];
    FILE *fp;
    size_t read_bytes;


    name_val = val_find_child(vir_val->parent,
                              SCOPE_MOD,
                              "name");
    assert(name_val);


#if 1
    sprintf(buf, "/usr/bin/lsi-acquisition-data-get %s", VAL_STRING(name_val));


    printf("Calling: %s\n", buf);

    if ((fp = popen(buf, "r")) == NULL) {
        printf("Error opening pipe!\n");
        assert(0);
    }

    read_bytes = fread(buf, 1, BUFSIZE-1, fp);
    if(read_bytes<=0) {
    	return ERR_NCX_SKIPPED;
    }


    buf[read_bytes]=0;

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        assert(0);
    }
#if 1
    res = val_set_simval_obj(dst_val,
                              vir_val->obj,
                              buf);
#else
    res = val_set_cplxval_obj(dst_val,
                              vir_val->obj,
                              "<geo-location xmlns=\"http://example.com/ns/geo-location\"><latitude>40.73297</latitude><longitude>-74.007696</longitude></geo-location>");
#endif

#else
    sprintf(buf, "%s-signal.wav", VAL_STRING(name_val));
    f = fopen(filename, "r");
    if (f == NULL) {
       return ERR_NCX_SKIPPED; 
    }
    read_bytes = fread(buf, BUFSIZE, 1, f);
    assert(read_bytes>0);
    fclose(f);

#endif
    /* disable cache */
    vir_val->cachetime = 0;

    return res;
}


static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *acquisition_cur_val;
    val_value_t *acquisition_new_val;
    val_value_t *acquisition_val;
    val_value_t *channels_val;
    val_value_t *channel_val;
    val_value_t *name_val;
    val_value_t *samples_val;
    val_value_t *parameters_val;
    val_value_t *data_val=NULL;

    unsigned int i;
    FILE* f;
    char buf[BUFSIZE];
    char* ptr;

    if(config_new_val == NULL) {
        return NO_ERR;
    }

    acquisition_new_val = val_find_child(config_new_val,
                               SCOPE_MOD,
                               "acquisition");

    if(acquisition_new_val == NULL) {
        return NO_ERR;
    }

    if(config_cur_val != NULL) {
        acquisition_cur_val = val_find_child(config_cur_val,
                               SCOPE_MOD,
                               "acquisition");

        if(acquisition_cur_val) {
            if(0==val_compare_ex(acquisition_cur_val, acquisition_new_val, TRUE)) {
                /*no change*/
                return NO_ERR;
            }
        }
    }
    acquisition_val = acquisition_new_val;


    samples_val = val_find_child(acquisition_val,
                               SCOPE_MOD,
                               "samples");
    channels_val = val_find_child(acquisition_val,
                               SCOPE_MOD,
                               "channels");
    if(channels_val == NULL) {
        return NO_ERR;
    }


    if(channels_val!=NULL) {
        for (channel_val = val_get_first_child(channels_val);
             channel_val != NULL;
             channel_val = val_get_next_child(channel_val)) {
            name_val = val_find_child(channel_val,
                               SCOPE_MOD,
                               "name");

            data_val = val_find_child(channel_val,
                               SCOPE_MOD,
                               "data");

            if(data_val != NULL) {
            	printf("Virtual node /acquisition/channels/channel/data already exists. Skipping.\n");
            	continue;
            }                                                                                                              

            data_obj = obj_find_child(channel_val->obj,
                             SCOPE_MOD,
                             "data");
            assert(data_obj);

            data_val = val_new_value();
            assert(data_val);

            val_init_virtual(data_val,
                         get_data,
                         data_obj);

            val_add_child(data_val, channel_val);


            parameters_val = val_find_child(channel_val,
                               SCOPE_MOD,
                               "parameters");


            sprintf(buf, "rm /tmp/%s-signal.wav", VAL_STRING(name_val));
            system(buf);
            sprintf(buf, "lsi-ivi-scope-acquisition-start \"%s\" %" PRIu64 " %s", VAL_STRING(name_val), VAL_UINT64(samples_val), parameters_val?VAL_STRING(parameters_val):"");

            printf("Calling: %s\n", buf);
            system(buf);

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
        if(0==val_compare_ex(cur_root_val,prev_root_val,TRUE)) {
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

/* The 3 mandatory callback functions: y_lsi_ivi_scope_init, y_lsi_ivi_scope_init2, y_lsi_ivi_scope_cleanup */

status_t
    y_lsi_ivi_scope_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t* agt_profile;
    status_t res;
    ncx_module_t *mod;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        SCOPE_MOD,
        NULL,
        &agt_profile->agt_savedevQ,
        &mod);
    if (res != NO_ERR) {
        return res;
    }

    res=agt_commit_complete_register("lsi-ivi-scope" /*SIL id string*/,
                                     y_commit_complete);
    assert(res == NO_ERR);

    return res;
}

status_t y_lsi_ivi_scope_init2(void)
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

void y_lsi_ivi_scope_cleanup (void)
{
}
