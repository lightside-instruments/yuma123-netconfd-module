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
#include "agt_fd_event_cb.h"
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
static obj_template_t* acquisition_complete_obj;
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

    sprintf(buf, "lsi-ivi-scope-acquisition-data-get %s", VAL_STRING(name_val));


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
    res = val_set_simval_obj(dst_val,
                              vir_val->obj,
                              buf);

    /* disable cache */
    vir_val->cachetime = 0;

    return res;
}

static void send_acquisition_complete_notification(void)
{
    status_t res;
    obj_template_t* notification_obj;
    agt_not_msg_t *notif;


    notification_obj = acquisition_complete_obj;


    notif = agt_not_new_notification(notification_obj);
    assert (notif != NULL);
    agt_not_queue_notification(notif);
}

int my_fd_event_cb_fn(int fd)
{
    send_acquisition_complete_notification();
    agt_fd_event_cb_unregister(fd);

    return 0;
}

static int update_config(val_value_t* config_cur_val, val_value_t* config_new_val)
{

    status_t res;

    val_value_t *acquisition_cur_val;
    val_value_t *acquisition_new_val;
    val_value_t *acquisition_val;
    val_value_t *trigger_val;
    val_value_t *trigger_slope_val;
    val_value_t *trigger_level_val;
    val_value_t *trigger_source_val;
    val_value_t *channels_val;
    val_value_t *channel_val;
    val_value_t *name_val;
    val_value_t *sample_rate_val;
    val_value_t *samples_val;
    val_value_t *parameters_val;
    val_value_t *range_val;
    val_value_t *data_val=NULL;

    char* sample_rate_str;
    char* trigger_level_str;
    char* range_str;

    unsigned int i;
    FILE* f;
    char cmd_buf[BUFSIZE];
    char rm_cmd_buf[BUFSIZE];
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


    sample_rate_val = val_find_child(acquisition_val,
                               SCOPE_MOD,
                               "sample-rate");
    assert(sample_rate_val);

    sample_rate_str = val_make_sprintf_string(sample_rate_val);


    samples_val = val_find_child(acquisition_val,
                               SCOPE_MOD,
                               "samples");

    trigger_val = val_find_child(acquisition_val,
                       SCOPE_MOD,
                       "trigger");

    if(trigger_val) {
        trigger_level_val = val_find_child(trigger_val,
                       SCOPE_MOD,
                       "level");
        trigger_source_val = val_find_child(trigger_val,
                       SCOPE_MOD,
                       "source");
        trigger_slope_val = val_find_child(trigger_val,
                       SCOPE_MOD,
                       "slope");
    }

    channels_val = val_find_child(acquisition_val,
                               SCOPE_MOD,
                               "channels");
    if(channels_val == NULL) {
        return NO_ERR;
    }

    if(trigger_val) {
        trigger_level_str = val_make_sprintf_string(trigger_level_val);

    }
    sprintf(cmd_buf, "lsi-ivi-scope-acquisition-start %s %" PRIu64 " %s %s %s", sample_rate_str, VAL_UINT64(samples_val), trigger_source_val?VAL_STRING(trigger_source_val):"-",trigger_slope_val?VAL_STRING(trigger_slope_val):"-",trigger_level_val?trigger_level_str:"0.0");
    free(sample_rate_str);

    if(trigger_val) {
       free(trigger_level_str);   	
    }

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


        range_val = val_find_child(channel_val,
                           SCOPE_MOD,
                           "range");


        parameters_val = val_find_child(channel_val,
                           SCOPE_MOD,
                           "parameters");


        sample_rate_str = val_make_sprintf_string(sample_rate_val);

        if(range_val) {
            range_str = val_make_sprintf_string(range_val);
        }

        sprintf(cmd_buf+strlen(cmd_buf), " %s %s \"%s\"", VAL_STRING(name_val), range_str, parameters_val?VAL_STRING(parameters_val):"\" \"");

        if(range_val) {
           free(range_str);
        }

        sprintf(rm_cmd_buf, "rm /tmp/%s-signal.wav", VAL_STRING(name_val));
        system(rm_cmd_buf);
    }

//    sprintf(cmd_buf+strlen(cmd_buf), " &");
    printf("Calling: %s\n", cmd_buf);

//    system(cmd_buf);
    {
        int     fd_in[2];
        int     fd_out[2];
        pid_t   childpid;

        pipe(fd_in);
        pipe(fd_out);

        childpid = fork();

        if(childpid == -1) {
            perror("fork");
            assert(0);
        } else if(childpid == 0) {
            dup2(fd_out[0], 0);
            close(fd_out[0]);
            close(fd_out[1]);
            dup2(fd_in[1], 1);
            close(fd_in[0]);
            close(fd_in[1]);
            execl("/bin/sh", "sh", "-c", cmd_buf, (char *) 0);
        } else {

            close(fd_out[0]);
            close(fd_in[1]);

            /* when the process terminates the select loop will execute fd_event_cb_fn */
            agt_fd_event_cb_register(fd_in[0], my_fd_event_cb_fn);

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

    acquisition_complete_obj = ncx_find_object(mod,"acquisition-complete");
    assert(acquisition_complete_obj);

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
