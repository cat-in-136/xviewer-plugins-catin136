# Sort Photo Plugin for xviewer
# Copyright (C) 2023  @cat_in_136
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

from gi.repository import GObject, Gtk, Xviewer

_UI_STR = """
<ui>
    <menubar name="MainMenu">
        <menu name="ToolsMenu" action="Tools">
            <separator/>
            <menu action="SortPhoto">
                <menuitem action="SortPhotoAscendingName"/>
                <menuitem action="SortPhotoDescendingName"/>
                <separator/>
                <menuitem action="SortPhotoAscendingMTime"/>
                <menuitem action="SortPhotoDescendingMTime"/>
            </menu>
            <separator/>
        </menu>
    </menubar>
</ui>
"""

class SortPhotoPlugin(GObject.Object, Xviewer.WindowActivatable):

    # Override XviewerWindowActivatable's window property
    window = GObject.property(type=Xviewer.Window)

    def __init__(self):
        GObject.Object.__init__(self)
        self.ui_id = 0

    def do_activate(self):
        ui_manager = self.window.get_ui_manager()
        self.action_group = Gtk.ActionGroup(name='SortPhoto')
        self.action_group.add_actions([
            ("SortPhoto", None, "Sort Photo",
             None, None, None),
            ("SortPhotoAscendingName", None, "Name (Ascending Order)",
             None, "Sort photo list by name in ascending order",
             self.on_ascending_name),
            ("SortPhotoDescendingName", None, "Name (Descending Order)",
             None, "Sort photo list by name in descending order",
             self.on_descending_name),
            ("SortPhotoAscendingMTime", None, "Modified Time (Ascending Order)",
             None, "Sort photo list by modified time in ascending order",
             self.on_ascending_mtime),
            ("SortPhotoDescendingMTime", None, "Modified Time (Descending Order)",
             None, "Sort photo list by modified time in descending order",
             self.on_descending_mtime),
            ], self.window)
        ui_manager.insert_action_group(self.action_group, 0)
        self.ui_id = ui_manager.add_ui_from_string(_UI_STR)

    def do_deactivate(self):
        ui_manager = self.window.get_ui_manager()
        ui_manager.remove_ui(self.ui_id)
        self.ui_id = 0
        ui_manager.remove_action_group(self.action_group)
        self.action_group = None
        ui_manager.ensure_update()

    def on_ascending_name(self, action, window):
        window.get_store().\
            set_default_sort_func(self.ascending_name_sort_func)

    @staticmethod
    def ascending_name_sort_func(store, iter1, iter2, data = None):
        pos1 = store[iter1][2].get_uri_for_display().lower()
        pos2 = store[iter2][2].get_uri_for_display().lower()

        if pos1 > pos2:
            return 1
        elif pos1 < pos2:
            return -1
        else:
            return 0

    def on_descending_name(self, action, window):
        window.get_store().\
            set_default_sort_func(self.descending_name_sort_func)

    @staticmethod
    def descending_name_sort_func(store, iter1, iter2, data = None):
        pos1 = store[iter1][2].get_uri_for_display().lower()
        pos2 = store[iter2][2].get_uri_for_display().lower()

        if pos1 > pos2:
            return -1
        elif pos1 < pos2:
            return +1
        else:
            return 0

    def on_ascending_mtime(self, action, window):
        window.get_store().\
            set_default_sort_func(self.ascending_mtime_sort_func)

    @staticmethod
    def ascending_mtime_sort_func(store, iter1, iter2, data = None):
        mtime1 = store[iter1][2].get_file().query_info("time::modified", 0, None).get_modification_date_time()
        mtime2 = store[iter2][2].get_file().query_info("time::modified", 0, None).get_modification_date_time()

        return mtime1.compare(mtime2)

    def on_descending_mtime(self, action, window):
        window.get_store().\
            set_default_sort_func(self.descending_mtime_sort_func)

    @staticmethod
    def descending_mtime_sort_func(store, iter1, iter2, data = None):
        mtime1 = store[iter1][2].get_file().query_info("time::modified", 0, None).get_modification_date_time()
        mtime2 = store[iter2][2].get_file().query_info("time::modified", 0, None).get_modification_date_time()

        return -mtime1.compare(mtime2)
