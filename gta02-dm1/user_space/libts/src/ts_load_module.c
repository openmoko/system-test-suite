/*
 *  tslib/src/ts_load_module.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_load_module.c,v 1.4 2004/10/19 22:01:27 dlowder Exp $
 *
 * Close a touchscreen device.
 */
#include "config.h"

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "tslib-private.h"

typedef struct tslib_module_info *(ts_builtin_init)(struct tsdev *, const char *);
extern ts_builtin_init mod_init_input_raw;
extern ts_builtin_init mod_init_pthres;
extern ts_builtin_init mod_init_variance;
extern ts_builtin_init mod_init_dejitter;
extern ts_builtin_init mod_init_linear;

static const struct builtin {
	const char *name;
	ts_builtin_init *init;
} builtins[] = {
	{ .name = "input",	.init = mod_init_input_raw },
	{ .name = "pthres",	.init = mod_init_pthres },
	{ .name = "variance",	.init = mod_init_variance },
	{ .name = "dejitter",	.init = mod_init_dejitter },
	{ .name = "linear",	.init = mod_init_linear },
	{ .name = NULL,		.init = NULL },
};

#if 0
int __ts_load_module(struct tsdev *ts, const char *module, const char *params, int raw)
{
	struct tslib_module_info * (*init)(struct tsdev *, const char *);
	struct tslib_module_info *info;
	char fn[1024];
	void *handle;
	int ret;
	char *plugin_directory=NULL;

	if( (plugin_directory = getenv("TSLIB_PLUGINDIR")) != NULL ) {
		//fn = alloca(sizeof(plugin_directory) + strlen(module) + 4);
		strcpy(fn,plugin_directory);
	} else {
		//fn = alloca(sizeof(PLUGIN_DIR) + strlen(module) + 4);
		strcpy(fn, PLUGIN_DIR);
	}

	strcat(fn, "/");
	strcat(fn, module);
	strcat(fn, ".so");

#ifdef DEBUG
	printf ("Loading module %s\n", fn);
#endif
	handle = dlopen(fn, RTLD_NOW);
	if (!handle)
		return -1;

	init = dlsym(handle, "mod_init");
	if (!init) {
		dlclose(handle);
		return -1;
	}

	info = init(ts, params);
	if (!info) {
		dlclose(handle);
		return -1;
	}

	info->handle = handle;

	if (raw) {
		ret = __ts_attach_raw(ts, info);
	} else {
		ret = __ts_attach(ts, info);
	}
	if (ret) {
		info->ops->fini(info);
		dlclose(handle);
	}

	return ret;
}


int ts_load_module(struct tsdev *ts, const char *module, const char *params)
{
	return __ts_load_module(ts, module, params, 0);
}

int ts_load_module_raw(struct tsdev *ts, const char *module, const char *params)
{
	return __ts_load_module(ts, module, params, 1);
}
#endif

static int __ts_load_builtin(struct tsdev *ts, const char *module, const char *params, int raw)
{
	const struct builtin *b;
	struct tslib_module_info *info;
	int ret = -1;

	for (b = builtins; b->name; b++)
	{
		if (strcmp(b->name, module) != 0)
			continue;

		info = b->init(ts, params);
		if (!info)
			return -1;

		info->handle = NULL;
		if (raw) {
			ret = __ts_attach_raw(ts, info);
		} else {
			ret = __ts_attach(ts, info);
		}
		if (ret) {
			info->ops->fini(info);
			return ret;
		}
	}

	return ret;
}

int ts_load_builtins(struct tsdev *ts)
{
	int ret;

	ret = __ts_load_builtin(ts, "input", "grab_events=1", 1);
	if (ret)
		return ret;

	ret = __ts_load_builtin(ts, "pthres", "pmin=1", 0);
	if (ret)
		return ret;

	ret = __ts_load_builtin(ts, "variance", "delta=30", 0);
	if (ret)
		return ret;

	ret = __ts_load_builtin(ts, "dejitter", "delta=100", 0);
	if (ret)
		return ret;

	ret = __ts_load_builtin(ts, "linear", NULL, 0);

	return ret;
}
