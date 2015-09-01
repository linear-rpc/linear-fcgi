#!/bin/sh

killall linear_fcgi
MOD_WEBSOCKET_CONFIG_PATH=/Users/kobota/webroot/etc/conf.d/linear.conf spawn-fcgi -f `pwd`/linear_fcgi -s /tmp/linear.fcgi.socket

# EOF
