#!/bin/bash
# added by taeguk.

kill `ps -fu $USER | grep -v grep | grep pintos | awk '{print $2}'`
#kill `ps -fu $USER | grep -v grep | grep qemu | awk '{print $2}'`
