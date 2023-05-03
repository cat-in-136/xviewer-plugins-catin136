#!/bin/bash

install_plugin () {
  local PLUGIN_NAME=$1

  /usr/bin/install -m 0644 -p ${PLUGIN_NAME}.plugin /usr/lib/xviewer/plugins/
  /usr/bin/install -m 0644 -p ${PLUGIN_NAME}.py /usr/lib/xviewer/plugins/
  /usr/bin/install -m 0644 -p xviewer-${PLUGIN_NAME}.metainfo.xml /usr/share/metainfo/
}

install_plugin sortphoto


