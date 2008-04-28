/* oltk.h
 *
 * (C) 2007 by OpenMoko, Inc.
 * Written by Chia-I Wu <olv@openmoko.com>
 * All Rights Reserved
 *
 * This file is part of oltk.
 *
 * oltk is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * oltk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with oltk; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 * 02110-1301 USA
 */ 

#ifndef _OLTK_H
#define _OLTK_H

#include "oltk_utils.h"

enum oltk_button_cb_type {
	OLTK_BUTTON_CB_CLICK,
	OLTK_BUTTON_CB_DRAW,
	N_OLTK_BUTTON_CBS
};

enum oltk_event_type {
	OLTK_EVENT_SAMPLE,
	N_OLTK_EVENTS
};

enum oltk_button_state_type {
	OLTK_BUTTON_STATE_NORMAL,
	OLTK_BUTTON_STATE_ACTIVE,
	N_OLTK_BUTTON_STATES,
};

enum oltk_button_color_type {
	OLTK_BUTTON_COLOR_FG,
	OLTK_BUTTON_COLOR_BG,
	OLTK_BUTTON_COLOR_BD,
	N_OLTK_BUTTON_COLORS,
};

struct oltk;
struct oltk_button;

struct oltk_event {
	enum oltk_event_type type;
	union {
		struct {
			int x;
			int y;
			int pressure;
		} sample;
	} u;
};

struct oltk_rectangle {
	int x;
	int y;
	int width;
	int height;
};

typedef void oltk_button_cb_click(struct oltk_button *button, void *data);
typedef void oltk_button_cb_draw(struct oltk_button *button,
		                 struct oltk_rectangle *rect, void *data);

OLTK_EXPORT
struct oltk_button *oltk_button_add(struct oltk *oltk,
		                    int x, int y,
				    int width, int height);
OLTK_EXPORT
void oltk_button_show(struct oltk_button *b, int show);
OLTK_EXPORT
void oltk_button_sensitive(struct oltk_button *b, int sensitive);
OLTK_EXPORT
void oltk_button_set_cb(struct oltk_button *b,
		        enum oltk_button_cb_type type,
			void *cb, void *data);
OLTK_EXPORT
void oltk_button_set_color(struct oltk_button *b, enum oltk_button_state_type state, enum oltk_button_color_type color, int rgb);
OLTK_EXPORT
void oltk_button_set_name(struct oltk_button *b, const char *name);
OLTK_EXPORT
void oltk_button_click(struct oltk_button *b);

struct oltk_view;
OLTK_EXPORT
struct oltk_view *oltk_view_add(struct oltk *,
		                int x, int y,
				int width, int height);
OLTK_EXPORT
void oltk_view_set_text(struct oltk_view *, const char *);
OLTK_EXPORT
void oltk_view_append_text(struct oltk_view *, const char *);

struct oltk_popup;
typedef void (*popup_select)(struct oltk_popup *pop,
		             int selected, void *data);

OLTK_EXPORT
struct oltk_popup *oltk_popup_add(struct oltk *,
		                  int, int, int, int, int);
OLTK_EXPORT
void oltk_popup_entry(struct oltk_popup *, int, const char *);
OLTK_EXPORT
void oltk_popup_set_cb(struct oltk_popup *,
		       popup_select callback, void *data);
OLTK_EXPORT
void oltk_popup_get_selected(struct oltk_popup *, int *);
OLTK_EXPORT
int oltk_popup_set_selected(struct oltk_popup *, int);

OLTK_EXPORT
struct oltk *oltk_new(const char *dev);
OLTK_EXPORT
void oltk_free(struct oltk *oltk);
OLTK_EXPORT
void oltk_get_resolution(struct oltk *oltk,
		         int *width, int *height);
OLTK_EXPORT
void oltk_set_theme(struct oltk *oltk, const int *fg, const int *bg, const int *bd);

OLTK_EXPORT
int oltk_listen(struct oltk *oltk, struct oltk_event *e);
OLTK_EXPORT
void oltk_abort(struct oltk *oltk);
OLTK_EXPORT
void oltk_feed(struct oltk *oltk, const struct oltk_event *e);
OLTK_EXPORT
void oltk_redraw(struct oltk *oltk);

OLTK_EXPORT
void oltk_msleep(struct oltk *oltk, unsigned int ms);
OLTK_EXPORT
void oltk_invalidate(struct oltk *oltk,
		     const struct oltk_rectangle *rect);

#endif /* _OLTK_H */
