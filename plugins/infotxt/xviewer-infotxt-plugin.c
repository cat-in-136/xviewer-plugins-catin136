#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <xviewer/xviewer-debug.h>
#include <xviewer/xviewer-image.h>
#include <xviewer/xviewer-job-scheduler.h>
#include <xviewer/xviewer-sidebar.h>
#include <xviewer/xviewer-thumb-view.h>
#include <xviewer/xviewer-window-activatable.h>
#include <xviewer/xviewer-window.h>

#include "xviewer-infotxt-plugin.h"

enum { PROP_0, PROP_WINDOW };

static void
xviewer_window_activatable_iface_init(XviewerWindowActivatableInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(
    XviewerInfotxtPlugin, xviewer_infotxt_plugin, PEAS_TYPE_EXTENSION_BASE, 0,
    G_IMPLEMENT_INTERFACE_DYNAMIC(XVIEWER_TYPE_WINDOW_ACTIVATABLE,
                                  xviewer_window_activatable_iface_init))

static void xviewer_infotxt_plugin_init(XviewerInfotxtPlugin *plugin) {
  xviewer_debug_message(DEBUG_PLUGINS, "XviewerInfotxtPlugin initializing");
}

static void xviewer_infotxt_plugin_finalize(GObject *object) {
  xviewer_debug_message(DEBUG_PLUGINS, "XviewerInfotxtPlugin finalizing");

  G_OBJECT_CLASS(xviewer_infotxt_plugin_parent_class)->finalize(object);
}

static void infotxt_copy_btn_cb(GtkWidget *button, gpointer data) {
  const gchar *const text = (const gchar *)data;
  GdkDisplay *const display = gdk_display_get_default();
  GtkClipboard *const clipboard = gtk_clipboard_get_default(display);
  gtk_clipboard_set_text(clipboard, text, -1);
}

static void insert_infotxt_to_textbuffer(GtkTextView *const textview,
                                         GtkTextBuffer *const buffer,
                                         GtkTextIter *buffer_iter,
                                         const gchar *const key,
                                         const gchar *const val) {
  gtk_text_buffer_insert_with_tags_by_name(buffer, buffer_iter, key, -1, "key",
                                           NULL);
  // AUTOMATIC1111/stable-diffusion-webui
  if ((g_strcmp0(key, "tEXt::parameters") == 0) &&
      (strstr(val, "\nSteps: ") != NULL)) {

    GtkTextChildAnchor *const anchor =
        gtk_text_buffer_create_child_anchor(buffer, buffer_iter);

    GtkWidget *button =
        gtk_button_new_from_icon_name("edit-copy-symbolic", GTK_ICON_SIZE_MENU);
    g_signal_connect(button, "clicked", G_CALLBACK(infotxt_copy_btn_cb),
                     (gpointer)val);
    gtk_text_view_add_child_at_anchor(textview, button, anchor);
    gtk_widget_show_all(button);
    gtk_text_buffer_insert(buffer, buffer_iter, "\n", -1);

    const gchar *p = val;

    if ((p != NULL) && (strstr(p, "\nNegative prompt: ") != NULL)) {
      const gchar *const eo_prompt = strstr(p, "\nNegative prompt: ");
      gtk_text_buffer_insert(buffer, buffer_iter, p, eo_prompt - p);
      p = eo_prompt;

      const gchar *const so_neg_prompt =
          eo_prompt + strlen("\nNegative prompt:");
      gtk_text_buffer_insert_with_tags_by_name(
          buffer, buffer_iter, p, so_neg_prompt - p, "semikey", NULL);
      p = so_neg_prompt;
    }

    if ((p != NULL) && (strstr(p, "\nSteps: ") != NULL)) {
      const gchar *const eo_prompt = strstr(p, "\nSteps: ");
      gtk_text_buffer_insert(buffer, buffer_iter, p, eo_prompt - p);
      p = eo_prompt;

      while (1) {
        const gchar *const eo_other_param_key_name = strstr(p, ": ");
        if (eo_other_param_key_name == NULL) {
          break;
        }
        const gchar *const eo_other_param_key =
            eo_other_param_key_name + strlen(":");
        gtk_text_buffer_insert_with_tags_by_name(
            buffer, buffer_iter, p, eo_other_param_key - p, "semikey", NULL);
        p = eo_other_param_key;

        if (g_str_has_prefix(p, " \"") &&
            (strstr(p + strlen(" \""), "\"") != NULL)) {
          const gchar *const eo_escaped_str =
              strstr(p + strlen(" \""), "\"") + strlen("\"");
          gtk_text_buffer_insert(buffer, buffer_iter, p, eo_escaped_str - p);
          p = eo_escaped_str;
        }

        const gchar *const eo_other_param_value = strstr(p, ", ");
        if (eo_other_param_value == NULL) {
          break;
        }

        const gchar *const so_other_param_key_next =
            eo_other_param_value + strlen(", ");
        gtk_text_buffer_insert(buffer, buffer_iter, p,
                               so_other_param_key_next - p);
        p = so_other_param_key_next;
      }
    }

    if (p != NULL) {
      gtk_text_buffer_insert(buffer, buffer_iter, p, -1);
      p = NULL;
    }
  } else { // Other normal
    gtk_text_buffer_insert(buffer, buffer_iter, "\n", -1);
    gtk_text_buffer_insert(buffer, buffer_iter, val, -1);
  }
  gtk_text_buffer_insert(buffer, buffer_iter, "\n\n", -1);
}

static void manage_infotxt_data(XviewerInfotxtPlugin *plugin) {
  XviewerImage *const image = xviewer_window_get_image(plugin->window);
  g_return_if_fail(image != NULL);

  GdkPixbuf *const pbuf = xviewer_image_get_pixbuf(image);
  if (pbuf) {
    GHashTable *const options = gdk_pixbuf_get_options(pbuf);
    if (options) {
      GHashTableIter option_iter;
      const gchar *key = NULL;
      const gchar *val = NULL;

      GtkTextBuffer *const buffer =
          gtk_text_view_get_buffer(GTK_TEXT_VIEW(plugin->view));
      GtkTextIter buffer_iter;

      gtk_text_buffer_set_text(buffer, "", -1); // clear all text
      gtk_text_buffer_get_iter_at_offset(buffer, &buffer_iter, 0);

      g_hash_table_iter_init(&option_iter, options);
      while (g_hash_table_iter_next(&option_iter, (gpointer *)&key,
                                    (gpointer *)&val)) {
        insert_infotxt_to_textbuffer(GTK_TEXT_VIEW(plugin->view), buffer,
                                     &buffer_iter, key, val);
      }
      // options is not freed here.
    }
    g_object_unref(pbuf);
  }
}

static void manage_infotxt_data_cb(XviewerJob *job, gpointer data) {
  if (xviewer_job_is_cancelled(job)) {
    return;
  }
  g_return_if_fail(xviewer_job_is_finished(job));

  manage_infotxt_data(XVIEWER_INFOTXT_PLUGIN(data));
}

static void selection_changed_cb(XviewerThumbView *view,
                                 XviewerInfotxtPlugin *plugin) {
  if (!xviewer_thumb_view_get_n_selected(view)) {
    return;
  }

  XviewerImage *const image = xviewer_thumb_view_get_first_selected_image(view);
  g_return_if_fail(image != NULL);

  // If EXIF is loaded, all image metadata is assumed to be loaded.
  if (xviewer_image_has_data(image, XVIEWER_IMAGE_DATA_EXIF)) {
    manage_infotxt_data(plugin);
  } else {
    XviewerJob *const job = xviewer_job_load_new(image, XVIEWER_IMAGE_DATA_EXIF);
    g_signal_connect(G_OBJECT(job), "finished",
                     G_CALLBACK(manage_infotxt_data_cb), plugin);
    xviewer_job_scheduler_add_job(job);
    g_object_unref(G_OBJECT(job));
  }
}

static void impl_activate(XviewerWindowActivatable *activatable) {
  XviewerInfotxtPlugin *const plugin = XVIEWER_INFOTXT_PLUGIN(activatable);

  xviewer_debug(DEBUG_PLUGINS);

  plugin->view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(plugin->view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(plugin->view), GTK_WRAP_WORD);

  // Create tags for text buffer
  GtkTextBuffer *const buffer =
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(plugin->view));
  gtk_text_buffer_create_tag(buffer, "key", "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_buffer_create_tag(buffer, "semikey", "weight", PANGO_WEIGHT_SEMIBOLD,
                             NULL);

  plugin->sidebar_page = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(plugin->sidebar_page),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(plugin->sidebar_page), plugin->view);

  GtkWidget *const sidebar = xviewer_window_get_sidebar(plugin->window);
  xviewer_sidebar_add_page(XVIEWER_SIDEBAR(sidebar), _("Info Text"),
                           plugin->sidebar_page);
  gtk_widget_show_all(plugin->sidebar_page);

  GtkWidget *const thumbview = xviewer_window_get_thumb_view(plugin->window);
  plugin->selection_changed_id =
      g_signal_connect(G_OBJECT(thumbview), "selection-changed",
                       G_CALLBACK(selection_changed_cb), plugin);
  /* force display of data now */
  selection_changed_cb(XVIEWER_THUMB_VIEW(thumbview), plugin);
}

static void impl_deactivate(XviewerWindowActivatable *activatable) {
  XviewerInfotxtPlugin *const plugin = XVIEWER_INFOTXT_PLUGIN(activatable);

  xviewer_debug(DEBUG_PLUGINS);

  GtkWidget *const sidebar = xviewer_window_get_sidebar(plugin->window);
  xviewer_sidebar_remove_page(XVIEWER_SIDEBAR(sidebar), plugin->sidebar_page);

  GtkWidget *const thumbview = xviewer_window_get_thumb_view(plugin->window);
  g_signal_handler_disconnect(G_OBJECT(thumbview),
                              plugin->selection_changed_id);
}

static void xviewer_infotxt_plugin_dispose(GObject *object) {
  XviewerInfotxtPlugin *plugin = XVIEWER_INFOTXT_PLUGIN(object);

  if (plugin->window != NULL) {
    g_object_unref(plugin->window);
    plugin->window = NULL;
  }

  if (plugin->sidebar_page != NULL) {
    g_object_unref(plugin->sidebar_page);
    plugin->sidebar_page = NULL;
  }

  G_OBJECT_CLASS(xviewer_infotxt_plugin_parent_class)->dispose(object);
}

static void xviewer_infotxt_plugin_get_property(GObject *object, guint prop_id,
                                                GValue *value,
                                                GParamSpec *pspec) {
  XviewerInfotxtPlugin *plugin = XVIEWER_INFOTXT_PLUGIN(object);

  switch (prop_id) {
  case PROP_WINDOW:
    g_value_set_object(value, plugin->window);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void xviewer_infotxt_plugin_set_property(GObject *object, guint prop_id,
                                                const GValue *value,
                                                GParamSpec *pspec) {
  XviewerInfotxtPlugin *plugin = XVIEWER_INFOTXT_PLUGIN(object);

  switch (prop_id) {
  case PROP_WINDOW:
    plugin->window = XVIEWER_WINDOW(g_value_dup_object(value));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
xviewer_infotxt_plugin_class_init(XviewerInfotxtPluginClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);

  object_class->finalize = xviewer_infotxt_plugin_finalize;
  object_class->dispose = xviewer_infotxt_plugin_dispose;
  object_class->set_property = xviewer_infotxt_plugin_set_property;
  object_class->get_property = xviewer_infotxt_plugin_get_property;

  g_object_class_override_property(object_class, PROP_WINDOW, "window");
}

static void xviewer_window_activatable_iface_init(
    XviewerWindowActivatableInterface *iface) {
  iface->activate = impl_activate;
  iface->deactivate = impl_deactivate;
}

static void
xviewer_infotxt_plugin_class_finalize(XviewerInfotxtPluginClass *klass) {
  /* Dummy needed for G_DEFINE_DYNAMIC_TYPE_EXTENDED */
}

G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module) {
  xviewer_infotxt_plugin_register_type(G_TYPE_MODULE(module));
  peas_object_module_register_extension_type(
      module, XVIEWER_TYPE_WINDOW_ACTIVATABLE, XVIEWER_TYPE_INFOTXT_PLUGIN);
}
