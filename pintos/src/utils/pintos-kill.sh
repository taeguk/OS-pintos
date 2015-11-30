#!/bin/bash
# added by taeguk.

kill `ps -ef | grep cse20141500 | grep pintos | awk '{print $2}'`
