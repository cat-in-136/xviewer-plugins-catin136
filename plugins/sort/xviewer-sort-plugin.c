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

#include <xviewer/xviewer-list-store.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <xviewer/xviewer-window-activatable.h>
#include <xviewer/xviewer-window.h>

#include "xviewer-sort-plugin.h"

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
     G_CALLBACK(on_descending_mtime)}};

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

static gint ascending_name_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
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

static gint descending_name_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
                                      GtkTreeIter *iter2, gpointer data) {
  return -ascending_name_sort_func(model, iter1, iter2, data);
}

static gint ascending_mtime_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
                                      GtkTreeIter *iter2, gpointer data) {
  gint retval = 0;

  XviewerImage *img1 = NULL;
  XviewerImage *img2 = NULL;
  gtk_tree_model_get(model, iter1, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img1, -1);
  gtk_tree_model_get(model, iter2, XVIEWER_LIST_STORE_XVIEWER_IMAGE, &img2, -1);

  GFile *const gfile1 = xviewer_image_get_file(img1);
  GFile *const gfile2 = xviewer_image_get_file(img2);

  GFileInfo *const fileinfo1 =
      g_file_query_info(gfile1, G_FILE_ATTRIBUTE_TIME_MODIFIED,
                        G_FILE_QUERY_INFO_NONE, NULL, NULL);
  GFileInfo *const fileinfo2 =
      g_file_query_info(gfile2, G_FILE_ATTRIBUTE_TIME_MODIFIED,
                        G_FILE_QUERY_INFO_NONE, NULL, NULL);

  if ((fileinfo1 != NULL) && (fileinfo2 != NULL)) {
    GDateTime *const mdatetime1 =
        g_file_info_get_modification_date_time(fileinfo1);
    GDateTime *const mdatetime2 =
        g_file_info_get_modification_date_time(fileinfo2);

    const gint64 mtime1 = g_date_time_to_unix(mdatetime1);
    const gint64 mtime2 = g_date_time_to_unix(mdatetime2);

    if (mtime1 > mtime2) {
      retval = 1;
    } else if (mtime1 < mtime2) {
      retval = -1;
    } else {
      retval = 0;
    }

    g_date_time_unref(mdatetime1);
    g_date_time_unref(mdatetime2);
    g_object_unref(fileinfo1);
    g_object_unref(fileinfo2);
  }

  g_object_unref(gfile1);
  g_object_unref(gfile2);
  g_object_unref(img1);
  g_object_unref(img2);

  return retval;
}

static gint descending_mtime_sort_func(GtkTreeModel *model, GtkTreeIter *iter1,
                                       GtkTreeIter *iter2, gpointer data) {
  return -ascending_mtime_sort_func(model, iter1, iter2, data);
}

static void on_ascending_name(GtkAction *action, XviewerWindow *window) {
  XviewerListStore *const store = xviewer_window_get_store(window);

  gtk_tree_sortable_set_default_sort_func(
      GTK_TREE_SORTABLE(store), &ascending_name_sort_func, NULL, NULL);
}

static void on_descending_name(GtkAction *action, XviewerWindow *window) {
  XviewerListStore *const store = xviewer_window_get_store(window);

  gtk_tree_sortable_set_default_sort_func(
      GTK_TREE_SORTABLE(store), &descending_name_sort_func, NULL, NULL);
}

static void on_ascending_mtime(GtkAction *action, XviewerWindow *window) {
  XviewerListStore *const store = xviewer_window_get_store(window);

  gtk_tree_sortable_set_default_sort_func(
      GTK_TREE_SORTABLE(store), &ascending_mtime_sort_func, NULL, NULL);
}

static void on_descending_mtime(GtkAction *action, XviewerWindow *window) {
  XviewerListStore *const store = xviewer_window_get_store(window);

  gtk_tree_sortable_set_default_sort_func(
      GTK_TREE_SORTABLE(store), &descending_mtime_sort_func, NULL, NULL);
}
