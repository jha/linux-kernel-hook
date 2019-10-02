#!/bin/bash

# If this is executed with /bin/sh, $UID isn't populated so we will use
# id -u to get the current uid
L_UID=$(id -u)

if [ "$L_UID" -ne "0" ]; then
    echo "You must be uid=0"
    exit 1
fi

KBASE32=$(cat /proc/kallsyms | grep " ia32_sys_call_table" | awk '{ print $1 }')
KBASE64=$(cat /proc/kallsyms | grep " sys_call_table" | awk '{ print $1 }')
KBASEARM=$(cat /proc/kallsyms | grep " sys_call_table" | awk '{ print $1 }')

if [ "$(uname -m)" = "x86_64" ]; then
    insmod ./x86_64/lkh.ko kbase32="$KBASE32" kbase64="$KBASE64"
else
    insmod ./arm/lkh.ko kbaseArm="$KBASEARM"
fi