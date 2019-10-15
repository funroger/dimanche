#!/usr/bin/env python3

import argparse
#import di_android
import di_config
import di_log
import di_platform
import di_project
import json
import os
import pathlib
#import sys
import time


__DEFAULT_SETTINGS_RELATIVE_PATH = "build.settings"


def __load_defaults(reset: bool = False):
    defaults = {}
    os_name = di_platform.os_name()
    platform_name = di_platform.platform_name()

    # try to load a default settings file
    script_dir = os.path.dirname(__file__)
    default_settings_path = os.path.join(script_dir, __DEFAULT_SETTINGS_RELATIVE_PATH)
    if os.path.exists(default_settings_path) and os.path.isfile(default_settings_path):
        # load defaults from the file
        file = open(default_settings_path)
        defaults = json.load(file)

    # update missed defaults
    if not "config_path" in defaults:
        defaults["config_path"] = os.path.join("..", "config", os_name + "-" + platform_name)
    if not "target" in defaults:
        defaults["target"] = {}
    target = defaults["target"]
    if not "config" in target: target["config"] = "debug"
    if not "os" in target: target["os"] = os_name
    if not "platform" in target: target["platform"] = platform_name
    if not "toolset" in defaults: defaults["toolset"] = ""
    if not "verbosity" in defaults: defaults["verbosity"] = di_log.verbosity_to_string(di_log.VERBOSITY.DEFAULT)

    return defaults


def __save_defaults(defaults: dict):
    # open a file with default settings
    script_dir = os.path.dirname(__file__)
    default_settings_path = os.path.join(script_dir, __DEFAULT_SETTINGS_RELATIVE_PATH)
    # save the settings
    file = open(default_settings_path, "w")
    json.dump(defaults, file, indent = 2, separators = (",", ": "), sort_keys = True)


def parse_parameters(ctx):

    defaults = __load_defaults()
    default_target = defaults["target"]

    # initialize argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument("project_path", type = str, help = "path to a project to build")
    parser.add_argument("--config", type = str, help = "configuration to build \
        [debug, release] (default is %s)" % default_target["config"],
        default = default_target["config"])
    parser.add_argument("--config_path", type = str, help = "path to config files \
        (default is %s)" % defaults["config_path"],
        default = defaults["config_path"])
    parser.add_argument("--defaults", type = str, help = "default values are to \
        (update, reset, '')",
        default = "")
    parser.add_argument("--os", type = str, help="target OS to build \
        [android, ios, linux, darwin, windows] (default is %s)" % default_target["os"],
        default = default_target["os"])
    parser.add_argument("--platform", type = str, help = "platform to build \
        [arm-v7a, arm-v8a, x86_64, x86] (default is %s)" % default_target["platform"],
        default = default_target["platform"])
    parser.add_argument("--toolset", type = str, help = "toolset to use \
        (default toolset depends on a platform)",
        default = defaults["toolset"])
    parser.add_argument("--verbosity",type = str, help = "verbosity level \
        [max, info, warning, error, message, silent] (default is %s)" % defaults["verbosity"],
        default = defaults["verbosity"])
    args = parser.parse_args()
    # save actual values
    ctx["args"] = args

    if "update" == args.defaults:
        defaults["config_path"] = args.config_path
        default_target["config"] = args.config
        default_target["os"] = args.os
        default_target["platform"] = args.platform
        defaults["toolset"] = args.toolset
        defaults["verbosity"] = args.verbosity
        __save_defaults(defaults)


def load_configuration(ctx):
    log = ctx["log"]

    # build a configuration file name
    script_dir = os.path.dirname(__file__)
    config_path = ctx["args"].config_path
    config_file_name = ctx["args"].os + "-" + ctx["args"].platform + "-" + \
        ctx["args"].config + ".cfg"
    config_file_path = os.path.join(script_dir, config_path, config_file_name)
    config_file_path = os.path.abspath(os.path.expandvars(config_file_path))

    config = di_config.load_config(config_file_path, log)
    ctx["config"] = config
    log.log(di_log.VERBOSITY.MAX, config)

    log.log(di_log.VERBOSITY.MESSAGE, "config file %s loaded" % config_file_name)


def load_project(ctx):
    log = ctx["log"]

    # build a project file name
    project_path = ctx["args"].project_path
    cwd = os.getcwd()
    project_file_path = os.path.join(cwd, project_path)
    project_file_path = os.path.abspath(os.path.expandvars(project_file_path))

    begin = time.perf_counter()
    project = di_project.load_project(project_file_path, log)
    end = time.perf_counter()
    log.log(di_log.VERBOSITY.INFO, "project loaded in %.03f seconds" % (end - begin))

def main():

    di_platform.clear()

    ctx = {}
    parse_parameters(ctx)

    # create a logging object
    verbosity = ctx["args"].verbosity
    log = di_log.Log(di_log.string_to_verbosity(verbosity))
    ctx["log"] = log
    log.log(di_log.VERBOSITY.MAX, ctx)

    log.log(di_log.VERBOSITY.INFO, "target os %s" % ctx["args"].os)
    log.log(di_log.VERBOSITY.INFO, "target platform %s" % ctx["args"].platform)
    log.log(di_log.VERBOSITY.INFO, "target configuration %s" % ctx["args"].config)

    load_configuration(ctx)

    # build a dependency tree
    load_project(ctx)

    # start building
   
main()
