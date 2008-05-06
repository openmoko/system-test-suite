/* oltk.c
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#else /* assume that we are building for u-boot */
#include <malloc.h>
#include <command.h>
#include <common.h>
#include <linux/string.h>

#define usleep udelay

#endif

#include "gr.h"
#include "oltk.h"
#include "font.h"

struct oltk {
	struct gr *gr;
	struct oltk_button **zbuttons;
	int n_buttons;
	int zsize;

	int abort;
	struct oltk_button *depressed;

	int fg[N_OLTK_BUTTON_STATES];
	int bg[N_OLTK_BUTTON_STATES];
	int bd[N_OLTK_BUTTON_STATES];
	int color_index;

	struct oltk_rectangle invalid_rect;
};

struct oltk_button {
	struct oltk_rectangle rect;
	int show;
	int sensitive;
	const char *name;

	int state;

	void *callbacks[N_OLTK_BUTTON_CBS];
	void *data[N_OLTK_BUTTON_CBS];

	int fg[N_OLTK_BUTTON_STATES];
	int bg[N_OLTK_BUTTON_STATES];
	int bd[N_OLTK_BUTTON_STATES];

	struct oltk *oltk;
	int update;
};

#define HAS_POINT(rect, px, py) \
	((px) >= (rect)->x && \
	 (px) < (rect)->x + (rect)->width && \
	 (py) >= (rect)->y && \
	 (py) < (rect)->y + (rect)->height )


static void oltk_draw_line(struct oltk *oltk, int x1, int y1, int x2, int y2, int color);
static void oltk_draw_rectangle(struct oltk *oltk, int x, int y, int width, int height, int fill, int color);
static void oltk_draw_point(struct oltk *oltk, int x, int y, int color);
static void oltk_draw_string(struct oltk *oltk, const char *str, int x, int y, int color);
static void oltk_string_extents(struct oltk *oltk, const char *str, int *width, int *height);
static void put_string_center(struct oltk *oltk, int x, int y, const char *s, unsigned color);

static void on_button_draw(struct oltk_button *b, struct oltk_rectangle *rect, void *data)
{
	oltk_draw_rectangle(b->oltk, b->rect.x, b->rect.y, b->rect.width, b->rect.height, 1, b->bg[b->state]);
	oltk_draw_rectangle(b->oltk, b->rect.x, b->rect.y, b->rect.width, b->rect.height, 0, b->bd[b->state]);

	if (b->name)
		put_string_center(b->oltk, b->rect.x + b->rect.width / 2, b->rect.y + b->rect.height / 2, b->name, b->fg[b->state]);
}

struct oltk_button *oltk_button_add(struct oltk *oltk, int x, int y, int width, int height)
{
	struct oltk_button *b;
	int i;
	
	if (oltk->n_buttons >= oltk->zsize)
	{
		struct oltk_button **new_buttons;
		int new_size = oltk->zsize * 2;

		new_buttons = realloc(oltk->zbuttons, sizeof(*new_buttons) * new_size);
		if (!new_buttons)
			return NULL;

		oltk->zbuttons = new_buttons;
		oltk->zsize = new_size;
	}

	b = malloc(sizeof(*b));
	if (!b)
		return NULL;

	b->oltk = oltk;
	b->rect.x = x;
	b->rect.y = y;
	b->rect.width = width;
	b->rect.height = height;
	b->state = OLTK_BUTTON_STATE_NORMAL;
	b->sensitive = 1;
	b->show = 0;
	b->update = 1;
	b->name = NULL;

	memcpy(b->fg, oltk->fg, sizeof(b->fg));
	memcpy(b->bg, oltk->bg, sizeof(b->bg));
	memcpy(b->bd, oltk->bd, sizeof(b->bd));

	for (i = 0; i < N_OLTK_BUTTON_CBS; i++)
	{
		b->callbacks[i] = NULL;
		b->data[i] = NULL;
	}

	b->callbacks[OLTK_BUTTON_CB_DRAW] = on_button_draw;

	oltk->zbuttons[oltk->n_buttons++] = b;

	return b;
}

void oltk_button_set_cb(struct oltk_button *b, enum oltk_button_cb_type type, void *cb, void *data)
{
	if (type < N_OLTK_BUTTON_CBS)
	{
		b->callbacks[type] = cb;
		b->data[type] = data;
	}
}

void oltk_button_set_color(struct oltk_button *b, enum oltk_button_state_type state, enum oltk_button_color_type color, int rgb)
{
	struct gr_rgb grrgb;
	struct gr *gr = b->oltk->gr;

	grrgb.red   = ((rgb >> 16) & 0xff) << 8;
	grrgb.green = ((rgb >>  8) & 0xff) << 8;
	grrgb.blue  = ((rgb      ) & 0xff) << 8;

	gr->set_color(gr, b->oltk->color_index, &grrgb, 1);
	rgb = gr->get_color(gr, b->oltk->color_index);
	b->oltk->color_index++;

	switch (color)
	{
	case OLTK_BUTTON_COLOR_FG:
		b->fg[state] = rgb;
		break;
	case OLTK_BUTTON_COLOR_BG:
		b->bg[state] = rgb;
		break;
	case OLTK_BUTTON_COLOR_BD:
		b->bd[state] = rgb;
		break;
	default:
		break;
	}
}

void oltk_button_sensitive(struct oltk_button *b, int sensitive)
{
	b->sensitive = sensitive;
}

void oltk_button_show(struct oltk_button *b, int show)
{
	if (b->show != show)
	{
		b->show = show;
		b->update = 1;

		if (!b->show)
			oltk_invalidate(b->oltk, &b->rect);
	}
}

void oltk_button_set_name(struct oltk_button *b, const char *name)
{
	b->name = name;
	b->update = 1;
}

void oltk_button_click(struct oltk_button *b)
{
	oltk_button_cb_click *click = b->callbacks[OLTK_BUTTON_CB_CLICK];

	if (click)
		click(b, b->data[OLTK_BUTTON_CB_CLICK]);
}

static int button_handle(struct oltk_button *b, const struct oltk_event *e)
{
	int old_state = b->state;
	int handled = 0;

	if (!b->show || !b->sensitive)
		return 0;

	if (e && e->type == OLTK_EVENT_SAMPLE && HAS_POINT(&b->rect, e->u.sample.x, e->u.sample.y))
	{
		if (e->u.sample.pressure)
		{
			if (b->state == OLTK_BUTTON_STATE_NORMAL)
				b->state = OLTK_BUTTON_STATE_ACTIVE;
		}
		else
		{
			if (b->state == OLTK_BUTTON_STATE_ACTIVE)
			{
				b->state = OLTK_BUTTON_STATE_NORMAL;
				oltk_button_click(b);
			}
		}

		handled = 1;
	}
	else
	{
		if (b->state == OLTK_BUTTON_STATE_ACTIVE)
			b->state = OLTK_BUTTON_STATE_NORMAL;
	}

	if (old_state != b->state)
		b->update = 1;

	return handled;
}

struct oltk_view {
	struct oltk_button *button;
	char *text;

	char **lines;
	int n_lines;

	int xpad, ypad;

	int xalign, yalign; /* [0, 100] */
};

static void draw_view(struct oltk_button *button, struct oltk_rectangle *rect, void *data)
{
	struct oltk_view *view = data;
	int x, y, width, height;
	int i;

	if (button->update)
		*rect = button->rect;

	oltk_draw_rectangle(button->oltk, button->rect.x, button->rect.y,  button->rect.width,  button->rect.height, 1, button->bg[button->state]);
	oltk_draw_rectangle(button->oltk, button->rect.x, button->rect.y,  button->rect.width,  button->rect.height, 0, button->bd[button->state]);

	if (!view->n_lines)
		return;

	width = height = 0;
	for (i = 0; i < view->n_lines; i++)
	{
		int w, h;

		oltk_string_extents(button->oltk, view->lines[i], &w, &h);

		if (w > width)
			width = w;

		height += h;
	}

	x = button->rect.x + view->xpad + (button->rect.width - 2 * view->xpad - width) * view->xalign / 100;
	y = button->rect.y + view->ypad + (button->rect.height - 2 * view->ypad - height) * view->yalign / 100;
	height /= view->n_lines;

	for (i = 0; i < view->n_lines; i++)
	{
		oltk_draw_string(button->oltk, view->lines[i], x, y, button->fg[button->state]);
		y += height;
	}
}

static void view_split_lines(struct oltk_view *view)
{
	char *p, *nl;
	int i;

	if (view->lines)
	{
		free(view->lines);
		view->lines = NULL;
	}
	view->n_lines = 0;

	if (!view->text)
		return;

	p = view->text;
	while ((nl = strchr(p, '\n')))
	{
		view->n_lines++;
		p = nl + 1;
	}
	if (*p)
		view->n_lines++;

	view->lines = malloc(sizeof(*view->lines) * view->n_lines);

	p = view->text;
	for (i = 0; i < view->n_lines; i++)
	{
		view->lines[i] = p;
		p = strchr(p, '\n');
		if (p)
			*p++ = '\0';
	}
}

struct oltk_view *oltk_view_add(struct oltk *oltk, int x, int y, int width, int height)
{
	struct oltk_view *view;

	view = malloc(sizeof(*view));
	if (!view)
		return NULL;

	view->button = oltk_button_add(oltk, x, y, width, height);
	view->text = NULL;
	view->lines = NULL;
	view->xpad = 5;
	view->ypad = 5;
	view->xalign = 50;
	view->yalign = 50;

	oltk_button_sensitive(view->button, 0);
	oltk_button_show(view->button, 1);

	oltk_button_set_cb(view->button, OLTK_BUTTON_CB_DRAW, draw_view, view);

	return view;
}

void oltk_view_set_text(struct oltk_view *view, const char *text)
{
	if (view->text)
		free(view->text);

	if (text)
		view->text = strdup(text);
	else
		view->text = NULL;

	view_split_lines(view);

	view->button->update = 1;
}

void oltk_view_append_text(struct oltk_view *view, const char *text)
{
	int old_len;
	char *p;

	if (!view->text)
	{
		oltk_view_set_text(view, text);

		return;
	}

	old_len = strlen(view->text);
	p = malloc(old_len + strlen(text));
	if (!p)
		return;

	strcpy(p, view->text);
	strcpy(p + old_len, text);

	free(view->text);
	view->text = p;

	view_split_lines(view);

	view->button->update = 1;
}

struct oltk_popup {
	struct oltk *oltk;
	struct oltk_button *button;
	popup_select select;
	void *data;

	struct oltk_button *dialog;
	struct oltk_button **items;
	int n_items;

	int run;
	int selected;
};

static void on_popdown(struct oltk_button *button, void *data)
{
	struct oltk_popup *pop = data;
	int i;

	pop->run = 0;

	for (i = 0; i < pop->n_items; i++)
	{
		if (button == pop->items[i])
		{
			pop->selected = i;

			break;
		}
	}
}

static void on_popup(struct oltk_button *button, void *data)
{
	struct oltk_popup *pop = data;
	int i;

	pop->run = 1;
	pop->selected = -1;

	oltk_button_show(pop->dialog, 1);
	for (i = 0; i < pop->n_items; i++)
		oltk_button_show(pop->items[i], 1);

	oltk_redraw(pop->oltk);

	while (pop->run)
	{
		struct oltk_event e, *pe;
		int ret;

		pe = &e;

		while ((ret = oltk_listen(pop->oltk, pe)) == 0);

		if (ret < 0)
			break;

		for (i = 0; i < pop->n_items; i++)
		{
			struct oltk_button *b = pop->items[i];

			if (button_handle(b, pe))
				pe = NULL;
		}

		if (pop->run)
			oltk_redraw(pop->oltk);
	}

	for (i = 0; i < pop->n_items; i++)
		oltk_button_show(pop->items[i], 0);
	oltk_button_show(pop->dialog, 0);

	oltk_popup_set_selected(pop, pop->selected);
}

struct oltk_popup *oltk_popup_add(struct oltk *oltk, int x, int y, int width, int height, int n_items)
{
	struct oltk_popup *pop;
	int i;
	int dialog_width, dialog_height;


	pop = malloc(sizeof(*pop));
	if (!pop)
		return NULL;

	pop->oltk = oltk;
	pop->select = NULL;

	pop->button = oltk_button_add(oltk, x, y, width, height);
	oltk_button_show(pop->button, 1);
	oltk_button_set_cb(pop->button, OLTK_BUTTON_CB_CLICK, on_popup, pop);

	pop->items = malloc(sizeof(*pop->items) * n_items);

	dialog_width =  (100 + 20 ) * (n_items/6+1) + 20;
	dialog_height = (30 + 40) * (n_items);
	(dialog_height >= (oltk->gr->height))? (dialog_height=oltk->gr->height) : dialog_height ;

	pop->dialog = oltk_button_add(oltk, x, y, dialog_width, dialog_height);
	oltk_button_sensitive(pop->dialog, 0);

	width = 100;
	height = 40;
	x = pop->dialog->rect.x + 40;
	y = pop->dialog->rect.y + 25;
	for (i = 0; i < n_items; i++)
	{
		pop->items[i] = oltk_button_add(oltk, x, y, width, height);
		oltk_button_set_cb(pop->items[i], OLTK_BUTTON_CB_CLICK, on_popdown, pop);
		y += height + 40;

		if(y >= (oltk->gr->height))
		{	
			x = pop->dialog->rect.x + 40*(n_items/6+1) + 100;
			y = pop->dialog->rect.y + 25;
		}
	}

	pop->n_items = n_items;

	return pop;
}

void oltk_popup_entry(struct oltk_popup *pop, int i, const char *name)
{
	if (i >= pop->n_items)
		return;

	oltk_button_set_name(pop->items[i], name);
}

void oltk_popup_set_cb(struct oltk_popup *pop, popup_select callback, void *data)
{
	pop->select = callback;
	pop->data = data;
}

void oltk_popup_get_selected(struct oltk_popup *pop, int *selected)
{
	*selected = pop->selected;
}

int oltk_popup_set_selected(struct oltk_popup *pop, int selected)
{
	if (selected < 0 || selected >= pop->n_items)
		return 0;

	pop->selected = selected;
	oltk_button_set_name(pop->button, pop->items[pop->selected]->name);
	pop->select(pop, pop->selected, pop->data);

	return 1;
}

struct oltk *oltk_new(const char *dev)
{
	struct oltk *oltk;

	const int default_theme[][N_OLTK_BUTTON_STATES] = {
		{ 0xffffff, 0x000000 },
		{ 0x304050, 0x80b8c0 },
		{ 0xffe080, 0xffe080 }
	};

	oltk  = malloc(sizeof(*oltk));
	if (!oltk) {
		printf("oltk_new: OOM\n");
		return NULL;
	}
	oltk->gr = gr_open(dev, 1);
	if (!oltk->gr)
	{
		printf("oltk_new: gr_open failed\n");
		free(oltk);

		return NULL;
	}

	oltk->zbuttons = malloc(sizeof(*oltk->zbuttons) * 64);
	if (!oltk->zbuttons)
	{
		oltk->gr->close(oltk->gr);
		free(oltk);

		return NULL;
	}

	oltk->zsize = 64;
	oltk->n_buttons = 0;

	oltk->abort = 0;
	oltk->depressed = NULL;

	oltk_draw_rectangle(oltk, 0, 0, oltk->gr->width, oltk->gr->height, 1, 0);

	oltk->color_index = 0;
	oltk_set_theme(oltk, default_theme[0], default_theme[1], default_theme[2]);

	oltk->invalid_rect.x = 0;
	oltk->invalid_rect.y = 0;
	oltk->invalid_rect.width = 0;
	oltk->invalid_rect.height = 0;

	return oltk;
}

void oltk_free(struct oltk *oltk)
{
	int i;

	for (i = 0; i < oltk->n_buttons; i++)
		free(oltk->zbuttons[i]);

	free(oltk->zbuttons);

	oltk->gr->close(oltk->gr);

	free(oltk);
}

void oltk_get_resolution(struct oltk *oltk, int *width, int *height)
{
	if (width)
		*width = oltk->gr->width;

	if (height)
		*height = oltk->gr->height;
}

void oltk_set_theme(struct oltk *oltk, const int *fg, const int *bg, const int *bd)
{
	int i;

	for (i = 0; i < N_OLTK_BUTTON_STATES; i++)
	{
		struct gr_rgb rgb[3];

		rgb[0].red   = ((fg[i] >> 16) & 0xff) << 8;
		rgb[0].green = ((fg[i] >>  8) & 0xff) << 8;
		rgb[0].blue  = ((fg[i]      ) & 0xff) << 8;

		rgb[1].red   = ((bg[i] >> 16) & 0xff) << 8;
		rgb[1].green = ((bg[i] >>  8) & 0xff) << 8;
		rgb[1].blue  = ((bg[i]      ) & 0xff) << 8;

		rgb[2].red   = ((bd[i] >> 16) & 0xff) << 8;
		rgb[2].green = ((bd[i] >>  8) & 0xff) << 8;
		rgb[2].blue  = ((bd[i]      ) & 0xff) << 8;

		oltk->gr->set_color(oltk->gr, oltk->color_index, rgb, 3);

		oltk->fg[i] = oltk->gr->get_color(oltk->gr, oltk->color_index);
		oltk->bg[i] = oltk->gr->get_color(oltk->gr, oltk->color_index + 1);
		oltk->bd[i] = oltk->gr->get_color(oltk->gr, oltk->color_index + 2);

		oltk->color_index += 3;
	}
}

int oltk_listen(struct oltk* oltk, struct oltk_event *e)
{
	int ret = 0;
	struct gr_sample s;

	while (!ret)
	{
		if (oltk->abort)
			return -1;

		/* `select' doesn't work for some drivers of tslib */
#if 0
		if (oltk->gr->fd >= 0)
		{
			fd_set rfds;

			FD_ZERO(&rfds);
			FD_SET(oltk->gr->fd, &rfds);

			ret = select(oltk->gr->fd + 1, &rfds, NULL, NULL, NULL);
		}
		else
#endif
		{
			usleep(1000);
			ret = 1;
		}

		if (ret > 0)
		{
			ret = oltk->gr->sample(oltk->gr, &s);

			if (ret > 0)
			{
				e->type = OLTK_EVENT_SAMPLE;
				e->u.sample.x = s.x;
				e->u.sample.y = s.y;
				e->u.sample.pressure = s.pressure;
			}
		}
	}

	return ret;
}

void oltk_abort(struct oltk *oltk)
{
	oltk->abort = 1;
}

void oltk_feed(struct oltk *oltk, const struct oltk_event *e)
{
	int i;

	if (e->type != OLTK_EVENT_SAMPLE)
		return;

	for (i = oltk->n_buttons - 1; i >= 0; i--)
	{
		struct oltk_button *b = oltk->zbuttons[i];

		if (button_handle(b, e))
		{
			if (oltk->depressed && b != oltk->depressed)
				button_handle(oltk->depressed, NULL);

			oltk->depressed = (b->state == OLTK_BUTTON_STATE_ACTIVE)
				          ? b : NULL;

			return;
		}
	}

	oltk->depressed = NULL;
}

void oltk_msleep(struct oltk *oltk, unsigned int ms)
{
#ifdef STDC_HEADERS
	struct gr_sample s;
	struct timeval tv;
	unsigned int now, end;

	gettimeofday(&tv, NULL);
	now = tv.tv_sec * 1000 + (tv.tv_usec + 500) / 1000;
	end = now + ms;

	while (now < end)
	{
		usleep(1000);
		while (oltk->gr->sample(oltk->gr, &s) > 0);

		gettimeofday(&tv, NULL);
		now = tv.tv_sec * 1000 + (tv.tv_usec + 500) / 1000;
	}
#else
	struct gr_sample s;

	usleep(1000 * ms);
	while (oltk->gr->sample(oltk->gr, &s) > 0);
#endif /* STDC_HEADERS */
}

void oltk_invalidate(struct oltk *oltk, const struct oltk_rectangle *rect)
{
	int l, r, t, b;

	if (oltk->invalid_rect.width * oltk->invalid_rect.height == 0)
	{
		oltk->invalid_rect = *rect;

		return;
	}

	l = oltk->invalid_rect.x;
	r = oltk->invalid_rect.x + oltk->invalid_rect.width;
	t = oltk->invalid_rect.y;
	b = oltk->invalid_rect.y + oltk->invalid_rect.height;

	if (rect->x < l)
		l = rect->x;
	if (rect->x + rect->width > r)
		r = rect->x + rect->width;
	if (rect->y < t)
		t = rect->y;
	if (rect->y + rect->height > b)
		b = rect->y + rect->height;

	oltk->invalid_rect.x = l;
	oltk->invalid_rect.width = r - l;
	oltk->invalid_rect.y = t;
	oltk->invalid_rect.height = b - t;
}

static int rect_intersect(struct oltk_rectangle *rect1, const struct oltk_rectangle *rect2)
{
	int l, r, t, b;

	if (!rect1->width || !rect1->height || !rect2->width || !rect2->height)
		goto empty;

	l = rect1->x;
	r = rect1->x + rect1->width;
	t = rect1->y;
	b = rect1->y + rect1->height;

	if (l < rect2->x)
		l = rect2->x;

	if (r > rect2->x + rect2->width)
		r = rect2->x + rect2->width;

	if (l >= r)
		goto empty;

	if (t < rect2->y)
		t = rect2->y;

	if (b > rect2->y + rect2->height)
		b = rect2->y + rect2->height;

	if (t >= b)
		goto empty;

	rect1->x = l;
	rect1->y = t;
	rect1->width = r - l;
	rect1->height = b - t;

	return 1;

empty:
	rect1->width = 0;

	return 0;
}

void oltk_redraw(struct oltk *oltk)
{
	struct oltk_button *b;
	int i;

	if (oltk->invalid_rect.width * oltk->invalid_rect.height)
	{
		/*
		printf("invalid rectangle: %d %d %d %d\n", oltk->invalid_rect.x, oltk->invalid_rect.y, oltk->invalid_rect.width, oltk->invalid_rect.height);
		*/
		oltk_draw_rectangle(oltk, oltk->invalid_rect.x, oltk->invalid_rect.y, oltk->invalid_rect.width, oltk->invalid_rect.height, 1, 0);
	}

	for (i = 0; i < oltk->n_buttons; i++)
	{
		b = oltk->zbuttons[i];

		if (b->show)
		{
			struct oltk_rectangle rect = oltk->invalid_rect;

			if (b->update || rect_intersect(&rect, &b->rect))
			{
				oltk_button_cb_draw *draw = b->callbacks[OLTK_BUTTON_CB_DRAW];

				if (draw)
					draw(b, &rect, b->data[OLTK_BUTTON_CB_DRAW]);

				oltk_invalidate(oltk, &b->rect);
			}
		}

		b->update = 0;
	}

	oltk->gr->update(oltk->gr, (struct gr_rectangle *) &oltk->invalid_rect, 1);

	/* reset invalid rectangle */
	oltk->invalid_rect.width = 0;
}

static void put_string_center(struct oltk *oltk, int x, int y, const char *s, unsigned color)
{
	int w, h;

	oltk_string_extents(oltk, s, &w, &h);
        oltk_draw_string (oltk, s, x - w / 2, y - h / 2, color);
}

static inline void hline(struct gr *gr, int x, int y, unsigned int width, int pixel)
{
	if (y < 0 || y >= gr->height || x >= gr->width)
		return;

	if (x < 0)
	{
		if (x + width <= 0)
			return;

		width += x;
		x = 0;
	}

	if (x + width >= gr->width)
		width = gr->width - x;

	if (!pixel || gr->bytes_per_pixel == 1)
	{
		memset(gr->fb + gr->pitch * y + gr->bytes_per_pixel * x, pixel, gr->bytes_per_pixel * width);

		return;
	}

	switch (gr->bytes_per_pixel)
	{
	case 2:
		{
			u_int16_t *buf = gr->fb + gr->pitch * y + gr->bytes_per_pixel * x;

			while (width--)
				*buf++ = pixel;
		}
		break;
	case 4:
		{
			u_int32_t *buf = gr->fb + gr->pitch * y + gr->bytes_per_pixel * x;

			while (width--)
				*buf++ = pixel;
		}
		break;
	}
}

static void fill_rectangle(struct gr *gr, int x, int y, unsigned int width, unsigned int height, int color)
{
	int i;

	for (i = 0; i < height; i++)
		hline(gr, x, y + i, width, color);
}

static void oltk_draw_point(struct oltk *oltk, int x, int y, int color)
{
	hline(oltk->gr, x, y, 1, color);
}

static void oltk_draw_line(struct oltk *oltk, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, tmp, step;

#define SIGN(x) (((x) >= 0) ? 1 : -1)
#define ABS(x) (x * SIGN(x))

	dx = x2 - x1;
	dy = y2 - y1;

	step = SIGN(dx) * SIGN(dy);
	dx = ABS(dx);
	dy = ABS(dy);

	if (!dy)
	{
		hline(oltk->gr, (x1 < x2) ? x1 : x2, y1, dx + 1, color);

		return;
	}

	if (dx < dy)
	{
		if (y1 > y2)
		{
			tmp = x1;
			x1 = x2;
			x2 = tmp;

			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

		tmp = 0;
		while (y1 <= y2)
		{
			oltk_draw_point(oltk, x1, y1, color);

			tmp += dx;
			if (tmp > dy)
			{
				tmp -= dy;
				x1 += step;
			}

			y1++;
		}
	}
	else
	{
		if (x1 > x2)
		{
			tmp = x1;
			x1 = x2;
			x2 = tmp;

			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

		tmp = 0;
		while (x1 <= x2)
		{
			oltk_draw_point(oltk, x1, y1, color);

			tmp += dy;
			if (tmp > dx)
			{
				tmp -= dx;
				y1 += step;
			}

			x1++;
		}
	}
#undef ABS
#undef SIGN
}

static void oltk_draw_rectangle(struct oltk *oltk, int x, int y, int width, int height, int fill, int color)
{
	if (fill)
		fill_rectangle(oltk->gr, x, y, width, height, color);
	else
	{
		width--;
		height--;

		oltk_draw_line(oltk, x, y, x + width, y, color);
		oltk_draw_line(oltk, x, y + height, x + width, y + height, color);
		oltk_draw_line(oltk, x, y, x, y + height, color);
		oltk_draw_line(oltk, x + width, y, x + width, y + height, color);
	}
}

static inline void draw_char(struct oltk *oltk, int x, int y, char c, int color)
{
	int i, j, k, line;

	for (i = 0; i < oltk_font_fixed_8x13.height; i++)
	{
		for (j = 0; j < oltk_font_fixed_8x13.pitch; j++)
		{
			int bits;

			line = oltk_font_fixed_8x13.data[(oltk_font_fixed_8x13.height * c + i) * oltk_font_fixed_8x13.pitch + j];
			bits = (j == oltk_font_fixed_8x13.pitch - 1)
				? oltk_font_fixed_8x13.width - 8 * j : 8;

			for (k = 0; k < bits; k++)
			{
				if (line & 0x80)
					oltk_draw_point(oltk, x + 8 * j + k, y + i, color);

				line <<= 1;
			}
		}
	}
}

static void oltk_draw_string(struct oltk *oltk, const char *s, int x, int y, int color)
{
	while (*s)
	{
		draw_char(oltk, x, y, *s, color);

		x += oltk_font_fixed_8x13.width;
		s++;
	}
}

static void oltk_string_extents(struct oltk *oltk, const char *str, int *width, int *height)
{
	if (width)
		*width = strlen(str) * oltk_font_fixed_8x13.width;

	if (height)
		*height = oltk_font_fixed_8x13.height;
}
