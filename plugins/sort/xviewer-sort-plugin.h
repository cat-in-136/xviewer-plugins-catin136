/* Sort  -- Sort photos
 *
 * Copyright (C) 2023-2024 @cat_in_136
 *
 * Author: @cat_in_136
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __XVIEWER_SORT_PLUGIN_H__
#define __XVIEWER_SORT_PLUGIN_H__

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <libpeas/peas-extension-base.h>
#include <libpeas/peas-object-module.h>
#include <xviewer/xviewer-application.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define XVIEWER_TYPE_SORT_PLUGIN (xviewer_sort_plugin_get_type())
#define XVIEWER_SORT_PLUGIN(o)                                                 \
  (G_TYPE_CHECK_INSTANCE_CAST((o), XVIEWER_TYPE_SORT_PLUGIN, XviewerSortPlugin))
#define XVIEWER_SORT_PLUGIN_CLASS(k)	G_TYPE_CHECK_CLASS_CAST((k),      XVIEWER_TYPE_SORT_PLUGIN, XviewerSortPluginClass))
#define XVIEWER_IS_SORT_PLUGIN(o)                                              \
  (G_TYPE_CHECK_INSTANCE_TYPE((o), XVIEWER_TYPE_SORT_PLUGIN))
#define XVIEWER_IS_SORT_PLUGIN_CLASS(k)                                        \
  (G_TYPE_CHECK_CLASS_TYPE((k), XVIEWER_TYPE_SORT_PLUGIN))
#define XVIEWER_SORT_PLUGIN_GET_CLASS(o)                                       \
  (G_TYPE_INSTANCE_GET_CLASS((o), XVIEWER_TYPE_SORT_PLUGIN,                    \
                             XviewerSortPluginClass))

/* Private structure type */
typedef struct _XviewerSortPluginPrivate XviewerSortPluginPrivate;

/*
 * Main object structure
 */
typedef struct _XviewerSortPlugin XviewerSortPlugin;

struct _XviewerSortPlugin {
  PeasExtensionBase parent_instance;

  XviewerWindow *window;
  GtkActionGroup *action_group;
  guint ui_id;
};

/*
 * Class definition
 */
typedef struct _XviewerSortPluginClass XviewerSortPluginClass;

struct _XviewerSortPluginClass {
  PeasExtensionBaseClass parent_class;
};

/*
 * Public methods
 */
GType xviewer_sort_plugin_get_type(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* __XVIEWER_SORT_PLUGIN_H__ */
