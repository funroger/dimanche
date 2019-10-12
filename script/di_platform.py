#!/usr/bin/env python3

import di_log
import os
import platform
import subprocess
import sys


def clear(): 
    # for windows 
    if sys.platform == 'win32': 
        _ = os.system('cls') 
    # for mac and linux(here, os.name is 'posix') 
    else: 
        _ = os.system('clear')


def exit_on_error(message, log=None, modulename=None):
    if None != log:
        if None != modulename:
            log.log(di_log.INFO, modulename + ":")
        log.log(di_log.ERROR, message)
    sys.exit()


def execute(command, command_work_dir=None, log=None):
    result = subprocess.run(command, stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT, universal_newlines=True,
        cwd=command_work_dir).stdout
    if None != log:
        log.log(di_log.INFO, result)
    return result


def get_os_name():
    os_name = sys.platform
    if "win32" == os_name:
        return "windows"
    elif "darwin" == os_name:
        return "darwin"
    else:
        return "linux"


def get_platform_name():
    platform_name = platform.machine()
    if "i386" == platform_name:
        return "x86"
    elif "x86_64" == platform_name:
        return "x86_64"
    elif "arm" == platform_name:
        return "arm-v7a"
    elif "aarch64" == platform_name:
        return "arm-v8a"
    else:
        return ""
   