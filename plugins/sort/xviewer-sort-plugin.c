/* Sort  -- Sort photos
 *
 * Copyright (C) 2023 @cat_in_136
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <xviewer/xviewer-debug.h>
#include <xviewer/xviewer-image.h>
#include <xviewer/xviewer-list-store.h>
#include <xviewer/xviewer-thumb-view.h>
#include <xviewer/xviewer-window-activatable.h>
#include <xviewer/xviewer-window.h>

#include "xviewer-sort-plugin.h"

#ifndef HAVE_EXIF
#error HAVE_EXIF must be set
#endif

static void
xviewer_window_activatable_iface_init(XviewerWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(
    XviewerSortPlugin, xviewer_sort_plugin, PEAS_TYPE_EXTENSION_BASE, 0,
    G_IMPLEMENT_INTERFACE_DYNAMIC(XVIEWER_TYPE_WINDOW_ACTIVATABLE,
                                  xviewer_window_activatable_iface_init))

enum { PROP_0, PROP_WINDOW };

static void on_ascending_name(GtkAction *action, XviewerWindow *window);
static void on_descending_name(GtkAction *action, XviewerWindow *window);
static void on_ascending_mtime(GtkAction *action, XviewerWindow *window);
static void on_descending_mtime(GtkAction *action, XviewerWindow *window);
#ifdef HAVE_EXIF
static void on_ascending_shoot_time(GtkAction *action, XviewerWindow *window);
static void on_descending_shoot_time(GtkAction *action, XviewerWindow *window);
#endif

static const gchar UI_STR[] =
    "<ui>"
    "  <menubar name=\"MainMenu\">"
    "    <menu name=\"ToolsMenu\" action=\"Tools\">"
    "      <separator/>"
    "      <menu action=\"XviewerSortPlugin\">"
    "        <menuitem action=\"XviewerSortPluginAscendingName\"/>"
    "        <menuitem action=\"XviewerSortPluginDescendingName\"/>"
    "        <separator/>"
    "        <menuitem action=\"XviewerSortPluginAscendingMTime\"/>"
    "        <menuitem action=\"XviewerSortPluginDescendingMTime\"/>"
#ifdef HAVE_EXIF
    "        <separator/>"
    "        <menuitem action=\"XviewerSortPluginAscendingShootTime\"/>"
    "        <menuitem action=\"XviewerSortPluginDescendingShootTime\"/>"
#endif
    "      </menu>"
    "      <separator/>"
    "    </menu>"
    "  </menubar>"
    "</ui>";

static const GtkActionEntry ACTION_ENTRIES[] = {
    {"XviewerSortPlugin", NULL, N_("Sort Photo"), NULL, NULL, G_CALLBACK(NULL)},
    {"XviewerSortPluginAscendingName", NULL, N_("Name (Ascending Order)"), NULL,
     N_("Sort photo list by name in ascending order"),
     G_CALLBACK(on_ascending_name)},
    {"XviewerSortPluginDescendingName", NULL, N_("Name (Descending Order)"),
     NULL, N_("Sort photo list by name in descending order"),
     G_CALLBACK(on_descending_name)},
    {"XviewerSortPluginAscendingMTime", NULL,
     N_("Modified Time (Ascending Order)"), NULL,
     N_("Sort photo list by modified time in ascending order"),
     G_CALLBACK(on_ascending_mtime)},
    {"XviewerSortPluginDescendingMTime", NULL,
     N_("Modified Time (Descending Order)"), NULL,
     N_("Sort photo list by modified time in descending order"),
     G_CALLBACK(on_descending_mtime)},
#ifdef HAVE_EXIF
    {"XviewerSortPluginAscendingShootTime", NULL,
     N_("Shoot Time (Ascending Order)"), NULL,
     N_("Sort photo list by EXIF-based shoot time in ascending order"),
     G_CALLBACK(on_ascending_shoot_time)},
    {"XviewerSortPluginDescendingShootTime", NULL,
     N_("Shoot Time (Descending Order)"), NULL,
     N_("Sort photo list by EXIF-based shoot time in descending order"),
     G_CALLBACK(on_descending_shoot_time)},
#endif
};

static void xviewer_sort_plugin_init(XviewerSortPlugin *plugin) {
  plugin->action_group = NULL;
  plugin->ui_id = 0;
}

static void impl_activate(XviewerWindowActivatable *activatable) {
  XviewerSortPlugin *const plugin = XVIEWER_SORT_PLUGIN(activatable);
  GtkUIManager *const ui_manager =
      xviewer_window_get_ui_manager(plugin->window);

  plugin->action_group = gtk_action_group_new("XviewerSortPluginActionGroup");
  gtk_action_group_set_translation_domain(plugin->action_group,
                                          GETTEXT_PACKAGE);
  gtk_action_group_add_actions(plugin->action_group, ACTION_ENTRIES,
                               G_N_ELEMENTS(ACTION_ENTRIES), plugin->window);

  gtk_ui_manager_insert_action_group(ui_manager, plugin->action_group, -1);
  plugin->ui_id =
      gtk_ui_manager_add_ui_from_string(ui_manager, UI_STR, -1, NULL);
}

static void impl_deactivate(XviewerWindowActivatable *activatable) {
  XviewerSortPlugin *const plugin = XVIEWER_SORT_PLUGIN(activatable);
  GtkUIManager *const ui_manager =
      xviewer_window_get_ui_manager(plugin->window);

  gtk_ui_manager_remove_ui(ui_manager, plugin->ui_id);
  plugin->ui_id = 0;
  gtk_ui_manager_remove_action_group(ui_manager, plugin->action_group);
  plugin->action_group = NULL;
  gtk_ui_manager_ensure_update(ui_manager);
  g_clear_object(&(plugin->action_group));
}

static void xviewer_sort_plugin_get_property(GObject *object, guint prop_id,
                                             GValue *value, GParamSpec *pspec) {
  XviewerSortPlugin *plugin = XVIEWER_SORT_PLUGIN(object);

  switch (prop_id) {
  case PROP_WINDOW:
    g_value_set_object(value, plugin->window);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void xviewer_sort_plugin_set_property(GObject *object, guint prop_id,
                                             const GValue *value,
                                             GParamSpec *pspec) {
  XviewerSortPlugin *plugin = XVIEWER_SORT_PLUGIN(object);

  switch (prop_id) {
  case PROP_WINDOW:
    plugin->window = XVIEWER_WINDOW(g_value_dup_object(value));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void xviewer_sort_plugin_class_init(XviewerSortPluginClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);

  object_class->set_property = xviewer_sort_plugin_set_property;
  object_class->get_property = xviewer_sort_plugin_get_property;

  g_object_class_override_property(object_class, PROP_WINDOW, "window");
}

static void xviewer_sort_plugin_class_finalize(XviewerSortPluginClass *klass) {
  /* Dummy needed for G_DEFINE_DYNAMIC_TYPE_EXTENDED */
}

static void xviewer_window_activatable_iface_init(
    XviewerWindowActivatableInterface *iface) {
  iface->activate = impl_activate;
  iface->deactivate = impl_deactivate;
}

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module) {
  xviewer_sort_plugin_register_type(G_TYPE_MODULE(module));
  peas_object_module_register_extension_type(
      module, XVIEWER_TYPE_WINDOW_ACTIVATABLE, XVIEWER_TYPE_SORT_PLUGIN);
}

#ifdef HAVE_EXIF
static gint64 exif_get_unix_date(ExifData *exif, gint datetime_tag,
                                 gint timezone_tag) {
  int year, month, day, hour, minutes, seconds;
  GTimeZone *timezone = NULL;
  gchar exif_buffer[512];
  const gchar *buf_ptr = NULL;

  buf_ptr = xviewer_exif_data_get_value(exif, datetime_tag, exif_buffer,
                                        sizeof(exif_buffer));
  if (buf_ptr == NULL) {
    return 0;
  }

  const int result = sscanf(buf_ptr, "%d:%d:%d %d:%d:%d", &year, &month, &day,
                            &hour, &minutes, &seconds);
  if (result < 3 || !g_date_valid_dmy(day, month, year)) {
    xviewer_debug_message(DEBUG_PLUGINS, "Wrong EXIF Date Format: %s", buf_ptr);
    return 0;
  }
  if (result < 5) {
    hour = minutes = seconds = 0; // ignore time
  }

  buf_ptr = xviewer_exif_data_get_value(exif, timezone_tag, exif_buffer,
                                        sizeof(exif_buffer));
  if (buf_ptr != NULL) {
    timezone = g_time_zone_new_identifier(buf_ptr);
  }
  if (timezone == NULL) {
    timezone = g_time_zone_new_local();
  }

  GDateTime *datetime =
      g_date_time_new(timezone, year, month, day, hour, minutes, seconds);

  const gint64 unix_time = g_date_time_to_unix(datetime);

  g_date_time_unref(datetime);
  g_time_zone_unref(timezone);

  return unix_time;
}
#endif

static gint64 xviewer_image_get_date_unix(XviewerImage *image,
                                          gboolean use_exif, GError **error) {
  gint64 mtime = 0;

#ifdef HAVE_EXIF
  if (use_exif) {
    ExifData *const exif = xviewer_image_get_exif_info(image);

    if (exif != NULL) {
      mtime = exif_get_unix_date(exif, EXIF_TAG_DATE_TIME_ORIGINAL,
                                 EXIF_TAG_OFFSET_TIME_ORIGINAL);
      if (mtime == 0) {
        mtime = exif_get_unix_date(exif, EXIF_TAG_DATE_TIME_DIGITIZED,
                                   EXIF_TAG_OFFSET_TIME_DIGITIZED);
      }

      exif_data_unref(exif);
    }
  }
#endif

  if (mtime == 0) {
    GFile *const gfile = xviewer_image_get_file(image);

    GFileInfo *const fileinfo =
        g_file_query_info(gfile, G_FILE_ATTRIBUTE_TIME_MODIFIED,
                          G_FILE_QUERY_INFO_NONE, NULL, error);
    if (fileinfo != NULL) {
      GDateTime *const mdatetime =
          g_file_info_get_modification_date_time(fileinfo);

      mtime = g_date_time_to_unix(mdatetime);

      g_date_time_unref(mdatetime);
      g_object_unref(fileinfo);
    }
    g_object_unref(gfile);
  }

  return mtime;
}

static gint name_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
                           GtkTreeIter *iter2, gpointer data) {
  gint retval = 0;

  XviewerImage *img1 = NULL;
  XviewerImage *img2 = NULL;
  gtk_tree_model_get(model, iter1, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img1, -1);
  gtk_tree_model_get(model, iter2, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img2, -1);

  gchar *uri1 = xviewer_image_get_uri_for_display(img1);
  gchar *uri2 = xviewer_image_get_uri_for_display(img2);

  retval = g_strcmp0(uri1, uri2);

  g_free(uri1);
  g_free(uri2);
  g_object_unref(img1);
  g_object_unref(img2);

  return retval;
}

static gint mtime_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
                            GtkTreeIter *iter2, gpointer data) {
  XviewerImage *img1 = NULL;
  XviewerImage *img2 = NULL;
  gtk_tree_model_get(model, iter1, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img1, -1);
  gtk_tree_model_get(model, iter2, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img2, -1);

  GError *err = NULL;
  const gint64 mtime1 = xviewer_image_get_date_unix(img1, FALSE, &err);
  if (err != NULL) {
    xviewer_debug_message(DEBUG_PLUGINS, "Failed to get mdata: %s",
                          err->message);
    g_error_free(err);
    return 0;
  }
  const gint64 mtime2 = xviewer_image_get_date_unix(img2, FALSE, &err);
  if (err != NULL) {
    xviewer_debug_message(DEBUG_PLUGINS, "Failed to get mdata: %s",
                          err->message);
    g_error_free(err);
    return 0;
  }

  g_object_unref(img1);
  g_object_unref(img2);

  return CLAMP(mtime1 - mtime2, -1, 1);
}

#ifdef HAVE_EXIF
static gint shoot_time_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
                                 GtkTreeIter *iter2, gpointer data) {
  XviewerImage *img1 = NULL;
  XviewerImage *img2 = NULL;
  gtk_tree_model_get(model, iter1, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img1, -1);
  gtk_tree_model_get(model, iter2, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img2, -1);

  GError *err = NULL;
  const gint64 mtime1 = xviewer_image_get_date_unix(img1, TRUE, &err);
  if (err != NULL) {
    xviewer_debug_message(DEBUG_PLUGINS, "Failed to get mdata: %s",
                          err->message);
    g_error_free(err);
    return 0;
  }
  const gint64 mtime2 = xviewer_image_get_date_unix(img2, TRUE, &err);
  if (err != NULL) {
    xviewer_debug_message(DEBUG_PLUGINS, "Failed to get mdata: %s",
                          err->message);
    g_error_free(err);
    return 0;
  }

  g_object_unref(img1);
  g_object_unref(img2);

  return CLAMP(mtime1 - mtime2, -1, 1);
}
#endif

static inline void apply_sort(XviewerWindow *window,
                              GtkTreeIterCompareFunc sort_func,
                              gint sort_column_id, GtkSortType order) {
  XviewerListStore *const store = xviewer_window_get_store(window);

  gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE(store), sort_func,
                                          NULL, NULL);
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store), sort_column_id,
                                       order);

  GtkWidget *const thumbview = xviewer_window_get_thumb_view(window);
  xviewer_thumb_view_select_single(XVIEWER_THUMB_VIEW(thumbview),
                                   XVIEWER_THUMB_VIEW_SELECT_CURRENT);
}

static void on_ascending_name(GtkAction *action, XviewerWindow *window) {
  apply_sort(window, &name_sort_func, GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
             GTK_SORT_ASCENDING);
}

static void on_descending_name(GtkAction *action, XviewerWindow *window) {
  apply_sort(window, &name_sort_func, GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
             GTK_SORT_DESCENDING);
}

static void on_ascending_mtime(GtkAction *action, XviewerWindow *window) {
  apply_sort(window, &mtime_sort_func, GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
             GTK_SORT_ASCENDING);
}

static void on_descending_mtime(GtkAction *action, XviewerWindow *window) {
  apply_sort(window, &mtime_sort_func, GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
             GTK_SORT_DESCENDING);
}

#ifdef HAVE_EXIF
static void on_ascending_shoot_time(GtkAction *action, XviewerWindow *window) {
  apply_sort(window, &shoot_time_sort_func,
             GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_ASCENDING);
}

static void on_descending_shoot_time(GtkAction *action, XviewerWindow *window) {
  apply_sort(window, &shoot_time_sort_func,
             GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_DESCENDING);
}
#endif
