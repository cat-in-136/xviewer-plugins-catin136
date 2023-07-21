#ifndef __XVIEWER_INFOTXT_PLUGIN_H__
#define __XVIEWER_INFOTXT_PLUGIN_H__

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
#define XVIEWER_TYPE_INFOTXT_PLUGIN (xviewer_infotxt_plugin_get_type())
#define XVIEWER_INFOTXT_PLUGIN(o)                                              \
  (G_TYPE_CHECK_INSTANCE_CAST((o), XVIEWER_TYPE_INFOTXT_PLUGIN,                \
                              XviewerInfotxtPlugin))
#define XVIEWER_INFOTXT_PLUGIN_CLASS(k)                                        \
  (G_TYPE_CHECK_CLASS_CAST((k), XVIEWER_TYPE_INFOTXT_PLUGIN,                   \
                           XviewerInfotxtPluginClass))
#define XVIEWER_IS_INFOTXT_PLUGIN(o)                                           \
  (G_TYPE_CHECK_INSTANCE_TYPE((o), XVIEWER_TYPE_INFOTXT_PLUGIN))
#define XVIEWER_IS_INFOTXT_PLUGIN_CLASS(k)                                     \
  (G_TYPE_CHECK_CLASS_TYPE((k), XVIEWER_TYPE_INFOTXT_PLUGIN))
#define XVIEWER_INFOTXT_PLUGIN_GET_CLASS(o)                                    \
  (G_TYPE_INSTANCE_GET_CLASS((o), XVIEWER_TYPE_INFOTXT_PLUGIN,                 \
                             XviewerInfotxtPluginClass))

/* Private structure type */
typedef struct _XviewerInfotxtPluginPrivate XviewerInfotxtPluginPrivate;

/*
 * Main object structure
 */
typedef struct _XviewerInfotxtPlugin XviewerInfotxtPlugin;

struct _XviewerInfotxtPlugin {
  PeasExtensionBase parent_instance;

  XviewerWindow *window;

  GtkWidget *view;
  GtkWidget *sidebar_page;

  gulong selection_changed_id;
};

/*
 * Class definition
 */
typedef struct _XviewerInfotxtPluginClass XviewerInfotxtPluginClass;

struct _XviewerInfotxtPluginClass {
  PeasExtensionBaseClass parent_class;
};

/*
 * Public methods
 */
GType xviewer_infotxt_plugin_get_type(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT void peas_register_types(PeasObjectModule *module);

G_END_DECLS

#endif /* __XVIEWER_INFOTXT_PLUGIN_H__ */
