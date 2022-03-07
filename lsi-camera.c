/*
    module lsi-camera
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
static ncx_module_t *lsi_camera_mod;
static obj_template_t* cameras_obj;

#define BUFSIZE 1000000

/* Registered callback functions */

static status_t
    get_cameras(ses_cb_t *scb,
                         getcb_mode_t cbmode,
                         val_value_t *vir_val,
                         val_value_t *dst_val)
{
    status_t res;
    char* ptr;
    res = NO_ERR;

    /* /cameras/camera */

    char *cmd = "lsi-camera-cameras-get";

    char buf[BUFSIZE];
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
        printf("Error opening pipe!\n");
        assert(0);
    }

    ptr = fgets(buf, BUFSIZE, fp);
    assert(ptr!=NULL);

    printf("lsi-camera-cameras-get: %s", buf);

    assert(strlen(buf));

    if(pclose(fp))  {
        printf("Command not found or exited with error status\n");
        assert(0);
    }
#if 1
    res = val_set_cplxval_obj(dst_val,
                              vir_val->obj,
                              buf);
#else
    res = val_set_cplxval_obj(dst_val,
                              vir_val->obj,
                              "<geo-location xmlns=\"http://example.com/ns/geo-location\"><latitude>40.73297</latitude><longitude>-74.007696</longitude></geo-location>");
#endif
    /* disable cache */
    vir_val->cachetime = 0;

    return res;
}

/* The 3 mandatory callback functions: y_lsi_camera_init, y_lsi_camera_init2, y_lsi_camera_cleanup */

status_t
    y_lsi_camera_init (
        const xmlChar *modname,
        const xmlChar *revision)
{
    agt_profile_t *agt_profile;
    status_t res;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(
        "lsi-camera",
        NULL,
        &agt_profile->agt_savedevQ,
        &lsi_camera_mod);
    if (res != NO_ERR) {
        return res;
    }

    cameras_obj = ncx_find_object(
        lsi_camera_mod,
        "cameras");
    if (cameras_obj == NULL) {
        return SET_ERROR(ERR_NCX_DEF_NOT_FOUND);
    }

    return res;
}

status_t y_lsi_camera_init2(void)
{
    status_t res;
    cfg_template_t* runningcfg;
    val_value_t* cameras_val;
    obj_template_t* obj;

    res = NO_ERR;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    if (!runningcfg || !runningcfg->root) {
        return SET_ERROR(ERR_INTERNAL_VAL);
    }

    cameras_val = val_new_value();
    assert(cameras_val != NULL);

    val_init_virtual(cameras_val,
                     get_cameras,
                     cameras_obj);

    val_add_child(cameras_val, runningcfg->root);


    return res;
}

void y_lsi_camera_cleanup (void)
{
}
