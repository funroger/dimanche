#!/usr/bin/env python3

import argparse
import di_build_settings
import di_log
import di_platform
import di_project
import json
import os
import tempfile
import time


def parse_parameters(ctx):

    defaults = __load_defaults()
    default_target = defaults["target"]

    # initialize argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument("project_path", type = str, help = "path to a project to build")
    parser.add_argument("--defaults", type = str, help = "how treat default values",
        choices = (['update', 'reset']), default = "")
    parser.add_argument("--build_settings_path", type = str, help = "path to settings files \
        (default is %s)" % defaults["build_settings_path"],
        default = defaults["build_settings_path"])
    parser.add_argument("--output_root", type = str, help = "upper level folder to store temporal files \
        (default is %s)" % defaults["output_root"],
        default = defaults["output_root"])
    parser.add_argument("--source_root", type = str, help = "upper level folder with sources \
        (default is %s)" % defaults["source_root"],
        default = defaults["source_root"])
    parser.add_argument("--target_config", type = str, help = "target configuration to build \
        (default is %s)" % default_target["config"],
        choices = (['debug', 'release']), default = default_target["config"])
    parser.add_argument("--target_os", type = str, help = "target OS to build \
        (default is %s)" % default_target["os"],
        choices = (['android', 'ios', 'linux', 'macos', 'windows']), default = default_target["os"])
    parser.add_argument("--target_platform", type = str, help = "target platform to build \
        (default is %s)" % default_target["platform"],
        choices = (['arm-v7a', 'arm-v8a', 'x86_64', 'x86']), default = default_target["platform"])
    parser.add_argument("--toolset", type = str, help = "toolset to use \
        (default toolset depends on a platform)",
        default = defaults["toolset"])
    parser.add_argument("--verbosity", type = str, help = "verbosity level \
        (default is %s)" % defaults["verbosity"],
        choices = (['max', 'info', 'warning', 'error', 'message', 'silent']), default = defaults["verbosity"])
    args = parser.parse_args()

    if "update" == args.defaults:
        defaults["build_settings_path"] = args.build_settings_path
        defaults["output_root"] = args.output_root
        defaults["source_root"] = args.source_root
        default_target["config"] = args.target_config
        default_target["os"] = args.target_os
        default_target["platform"] = args.target_platform
        defaults["toolset"] = args.toolset
        defaults["verbosity"] = args.verbosity
        __save_defaults(defaults)

    # save actual values
    ctx["args"] = args
    # make real pathes
    ctx["args"].build_settings_path = os.path.expandvars(ctx["args"].build_settings_path)
    ctx["args"].output_root = os.path.expandvars(ctx["args"].output_root)
    ctx["args"].source_root = os.path.expandvars(ctx["args"].source_root)


__DEFAULT_SETTINGS_RELATIVE_PATH = os.path.split(__file__)[1] + \
    "." + di_platform.os_name() + ".settings"


def __load_defaults(reset: bool = False):
    defaults = {}
    os_name = di_platform.os_name()
    platform_name = di_platform.platform_name()
    output_root = di_platform.temp_dir()

    # try to load a default settings file
    script_dir = os.path.dirname(__file__)
    default_settings_path = os.path.join(script_dir, __DEFAULT_SETTINGS_RELATIVE_PATH)
    if os.path.exists(default_settings_path) and os.path.isfile(default_settings_path):
        # load defaults from the file
        file = open(default_settings_path)
        defaults = json.load(file)

    # update missed defaults
    if not "build_settings_path" in defaults: defaults["build_settings_path"] = os.path.join("..", "settings", os_name + "-" + platform_name)
    if not "output_root" in defaults: defaults["output_root"] = output_root
    if not "source_root" in defaults: defaults["source_root"] = "~"
    if not "target" in defaults: defaults["target"] = {}
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


def load_build_settings(ctx):
    log = ctx["log"]

    # build a settings file name
    script_dir = os.path.dirname(__file__)
    build_settings_path = ctx["args"].build_settings_path
    build_settings_file_name = ctx["args"].target_os + "-" + ctx["args"].target_platform + "-" + \
        ctx["args"].target_config + ".cfg"
    build_settings_file_path = os.path.join(script_dir, build_settings_path, build_settings_file_name)
    build_settings_file_path = os.path.abspath(os.path.expandvars(build_settings_file_path))

    build_settings = di_build_settings.load_build_settings(build_settings_file_path, log)
    ctx["build_settings"] = build_settings
    log.log(di_log.VERBOSITY.MAX, build_settings)

    log.log(di_log.VERBOSITY.MESSAGE, "build settings file %s loaded" % build_settings_file_name)


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

    log.log(di_log.VERBOSITY.INFO, "output root %s" % ctx["args"].output_root)
    log.log(di_log.VERBOSITY.INFO, "source root %s" % ctx["args"].source_root)
    log.log(di_log.VERBOSITY.INFO, "target os %s" % ctx["args"].target_os)
    log.log(di_log.VERBOSITY.INFO, "target platform %s" % ctx["args"].target_platform)
    log.log(di_log.VERBOSITY.INFO, "target configuration %s" % ctx["args"].target_config)

    load_build_settings(ctx)

    # build a dependency tree
    load_project(ctx)

    # start building


main()
