/*
    module lsi-display
    implementation for X11
    namespace urn:lsi:params:xml:ns:yang:lsi-display
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

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

#include "dict.h"
#include "decode.h"

#define DISPLAY_MOD "lsi-display"


/* module static variables */
static ncx_module_t* displayinterfaces_mod;
static ncx_module_t* iana_if_type_mod;
static ncx_module_t* display_mod;

static dlq_hdr_t io_dict;
typedef struct io_t_ {
    FILE* in;
    FILE* out;
} io_t;

static void display_delete(val_value_t * display_val)
{
    char cmd_buf[4096];
    static char cmd_args_buf[4096];
    val_value_t* name_val;
    io_t* io;
    char* name_buf;

    printf("display delete:\n");
    val_dump_value(display_val, NCX_DEF_INDENT);
    name_val = val_find_child(display_val, DISPLAY_MOD, "name");
    assert(name_val);

    io = dict_get_data(&io_dict, VAL_STRING(name_val));
    assert(io);

    fclose(io->in);
    fclose(io->out);

    name_buf = dict_get_name(&io_dict, io);
    dict_remove(&io_dict, VAL_STRING(name_val));
    free(name_buf);
    free(io);
}


static void display_create(val_value_t * display_val)
{
    char cmd_buf[4096];
    static char cmd_args_buf[4096 - 100];
    val_value_t* name_val;
    val_value_t* image_val;

    printf("display_create:\n");
    val_dump_value(display_val, NCX_DEF_INDENT);
    name_val = val_find_child(display_val, DISPLAY_MOD, "name");
    assert(name_val);
    image_val = val_find_child(display_val, DISPLAY_MOD, "image");
    assert(image_val);

    sprintf(cmd_args_buf, "--title=%s --width=%u --height=%u", VAL_STRING(name_val), 1920, 1080);
    sprintf(cmd_buf, "display-pipe %s", cmd_args_buf);

    //system(cmd_buf);
    {
	int fd_in[2];
	int fd_out[2];
	pid_t childpid;

	pipe(fd_in);
	pipe(fd_out);

	childpid = fork();

	if (childpid == -1) {
	    perror("fork");
	    assert(0);
	} else if (childpid == 0) {
	    dup2(fd_out[0], 0);
	    close(fd_out[0]);
	    close(fd_out[1]);
	    dup2(fd_in[1], 1);
	    close(fd_in[0]);
	    close(fd_in[1]);
	    execl("/usr/bin/display-pipe", cmd_buf, (char *) 0);
	} else {
	    int nbytes;
	    char readbuffer[1024];
	    char* lineptr;
	    ssize_t n;
	    ssize_t ret;
	    io_t* io;

	    io = malloc(sizeof(io_t));
	    assert(io);

	    close(fd_out[0]);
	    close(fd_in[1]);

	    io->out = fdopen(fd_out[1], "w");
	    assert(io->out != NULL);
	    io->in = fdopen(fd_in[0], "r");
	    assert(io->in != NULL);

	    dict_add(&io_dict, strdup(VAL_STRING(name_val)), (void *) io);
#if 1
	    {
		void* data = malloc(1920 * 1080 * 4);

		memset(data, 0x00, 1920*1080*4);
		image_decode(image_val->v.binary.ustr, image_val->v.binary.ustrlen, data);
		fwrite(data, 1920 * 1080 * 4, 1, io->out);
		fflush(io->out);
		free(data);
	    }
#endif
	}
    }
}

static void display_update(val_value_t * display_val)
{
    size_t ret;
    char cmd_buf[4096];
    static char cmd_args_buf[4096 - 100];
    val_value_t* name_val;
    val_value_t* image_val;
    io_t* io;

    printf("display_update:\n");
    val_dump_value(display_val, NCX_DEF_INDENT);
    name_val = val_find_child(display_val, DISPLAY_MOD, "name");
    assert(name_val);
    image_val = val_find_child(display_val, DISPLAY_MOD, "image");
    assert(image_val);

    io = dict_get_data(&io_dict, VAL_STRING(name_val));
    assert(io);
#if 1
    {
	void* data = malloc(1920 * 1080 * 4);
	memset(data, 0x00, 1920*1080*4);
	image_decode(image_val->v.binary.ustr, image_val->v.binary.ustrlen, data);
	fwrite(data, 1920 * 1080 * 4, 1, io->out);
	fflush(io->out);
	free(data);
    }
#endif

}

static int update_config(val_value_t * config_cur_val, val_value_t * config_new_val)
{

    status_t res;

    val_value_t* displays_cur_val,* display_cur_val,* name_cur_val;
    val_value_t* displays_new_val,* display_new_val,* name_new_val;


    if (config_new_val == NULL) {
	displays_new_val = NULL;
    } else {
	displays_new_val = val_find_child(config_new_val, DISPLAY_MOD, "displays");
    }

    if (config_cur_val == NULL) {
	displays_cur_val = NULL;
    } else {
	displays_cur_val = val_find_child(config_cur_val, DISPLAY_MOD, "displays");
    }

    /* 2 step (delete/add) display configuration */

    /* 1. deactivation loop - deletes all deleted or modified display -s */
    if (displays_cur_val != NULL) {
	for (display_cur_val = val_get_first_child(displays_cur_val); display_cur_val != NULL; display_cur_val = val_get_next_child(display_cur_val)) {

	    name_cur_val = val_find_child(display_cur_val, DISPLAY_MOD, "name");
	    display_new_val = val123_find_match(config_new_val, display_cur_val);
	    if (display_new_val == NULL) {
		display_delete(display_cur_val);
	    }
	}
    }

    /* 2. activation loop - adds all new or modified display -s */
    if (displays_new_val != NULL) {
	for (display_new_val = val_get_first_child(displays_new_val); display_new_val != NULL; display_new_val = val_get_next_child(display_new_val)) {

	    name_new_val = val_find_child(display_new_val, DISPLAY_MOD, "name");
	    display_cur_val = val123_find_match(config_cur_val, display_new_val);
	    if (display_cur_val == NULL) {
		display_create(display_new_val);
	    } else if (0 != val_compare_ex(display_new_val, display_cur_val, TRUE)) {
		display_update(display_new_val);
	    }
	}
    }
    return NO_ERR;
}

static val_value_t* prev_root_val = NULL;
static int update_config_wrapper()
{
    cfg_template_t* runningcfg;
    status_t res;
    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg != NULL && runningcfg->root != NULL);
    if (prev_root_val != NULL) {
	val_value_t* cur_root_val;
	cur_root_val = val_clone_config_data(runningcfg->root, &res);
	if (0 == val_compare(cur_root_val, prev_root_val)) {
	    /*no change */
	    val_free_value(cur_root_val);
	    return 0;
	}
	val_free_value(cur_root_val);
    }
    update_config(prev_root_val, runningcfg->root);

    if (prev_root_val != NULL) {
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

/* The 3 mandatory callback functions: y_lsi_display_init, y_lsi_display_init2, y_lsi_display_cleanup */

status_t y_lsi_display_init(const xmlChar * modname, const xmlChar * revision)
{
    agt_profile_t* agt_profile;
    status_t res;

    agt_profile = agt_get_profile();

    res = ncxmod_load_module(DISPLAY_MOD, NULL, &agt_profile->agt_savedevQ, &display_mod);
    if (res != NO_ERR) {
	return res;
    }
    //agt_disable_feature("ietf-traffic-generator", "ingress-direction");

    res = agt_commit_complete_register(DISPLAY_MOD /*SIL id string */ ,
				       y_commit_complete);
    assert(res == NO_ERR);

    dict_init(&io_dict);

    return res;
}

status_t y_lsi_display_init2(void)
{
    status_t res = NO_ERR;
    cfg_template_t* runningcfg;
    ncx_module_t* mod;
    obj_template_t* interfaces_obj;
    val_value_t* root_val;
    val_value_t* interfaces_val;

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg && runningcfg->root);
    root_val = runningcfg->root;

    y_commit_complete();

    return res;
}

void y_lsi_display_cleanup(void)
{
}
