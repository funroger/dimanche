#!/usr/bin/env python3

import di_log
import os
import platform
import shutil
import subprocess
import sys
import tempfile


def clear() -> None: 
    # for windows 
    if sys.platform == 'win32': 
        _ = os.system('cls') 
    # for mac and linux(here, os.name is 'posix') 
    else: 
        _ = os.system('clear')


def delete_file_tree(path: str, log = None) -> None:
    path = di_platform.expand_path(path)
    shutil.rmtree(path, ignore_errors = true)


def exit_on_error(message: str, modulename: str = None, log = None) -> None:
    if None != log:
        if None != modulename:
            log.log(di_log.VERBOSITY.INFO, modulename + ":")
        log.log(di_log.VERBOSITY.ERROR, message)
    sys.exit()


def execute(command, command_work_dir: str = None, log = None) -> str:
    result = subprocess.run(command, stdout = subprocess.PIPE,
        stderr = subprocess.STDOUT, universal_newlines = True,
        cwd = command_work_dir).stdout
    if None != log:
        log.log(di_log.VERBOSITY.INFO, result)
    return result


def expand_path(path: str) -> str:
    path = os.path.expandvars(path)
    path = os.path.expanduser(path)
    path = os.path.abspath(path)
    return path


def os_name() -> str:
    os_name = sys.platform
    if "win32" == os_name:
        return "windows"
    elif "darwin" == os_name:
        return "macos"
    else:
        return "linux"


def platform_name() -> str:
    platform_name = platform.machine()
    if "i386" == platform_name:
        return "x86"
    elif ("x86_64" == platform_name) or ("AMD64" == platform_name):
        return "x86_64"
    elif "arm" == platform_name:
        return "arm-v7a"
    elif "aarch64" == platform_name:
        return "arm-v8a"
    else:
        print(__file__ + ": unknown platform name")
        sys.exit()


def temp_dir() -> str:
    return '/tmp' if os_name() == 'macos' else tempfile.gettempdir()
