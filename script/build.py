#!/usr/bin/env python3

import argparse
#import di_android
import di_config
import di_log
import di_platform
import di_project
import os
import pathlib
#import sys
import time


def parse_parameters(ctx):
    os_name = di_platform.os_name()
    platform_name = di_platform.platform_name()

    default_config = "debug"
    default_os = os_name
    default_platform = platform_name
    default_verbosity = di_log.verbosity_to_string(di_log.VERBOSITY.DEFAULT)

    default_config_path = os.path.dirname(__file__) + "/" + "../config/" + \
        os_name + "-" + platform_name;
    default_config_path = os.path.abspath(default_config_path)

    # initialize argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument("project_path", type=str, help="path to a project to build") 
    parser.add_argument("--config", type=str, help="configuration to build \
        [debug, release] (default is %s)" % default_config,
        default=default_config) 
    parser.add_argument("--config_path", type=str, help="path to config files \
        (default is %s)" % default_config_path,
        default=default_config_path) 
    parser.add_argument("--os", type=str, help="target OS to build \
        [android, ios, linux, darwin, windows] (default is %s)" % default_os,
        default=default_os) 
    parser.add_argument("--platform", type=str, help="platform to build \
        [arm-v7a, arm-v8a, x86_64, x86] (default is %s)" % default_platform,
        default=default_platform)  
    parser.add_argument("--toolset", type=str, help="toolset to use \
        (default toolset depends on a platform)",
        default="")  
    parser.add_argument("--verbosity",type=str, help="verbosity level \
        [max, info, warning, error, message, silent] (default is %s)" % default_verbosity,
        default=default_verbosity)
    args = parser.parse_args()
    # save actual values
    ctx["args"] = args


def load_configuration(ctx):
    log = ctx["log"]

    # build a configuration file name
    config_file_name = ctx["args"].os + "-" + ctx["args"].platform + "-" + \
        ctx["args"].config + ".cfg"
    config_path = ctx["args"].config_path
    config_file_path = config_path + "/" + config_file_name
    config_file_path = os.path.abspath(config_file_path)

    config = di_config.load_config(config_file_path, log)
    ctx["config"] = config
    log.log(di_log.VERBOSITY.MAX, config)

    log.log(di_log.VERBOSITY.MESSAGE, "config file %s loaded" % config_file_name)


def load_project(ctx):
    log = ctx["log"]

    # build a project file name
    project_file_name = ctx["args"].project_path
    project_path = os.getcwd()
    project_file_path = project_path + "/" + project_file_name
    project_file_path = os.path.abspath(project_file_path)

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

    load_configuration(ctx)

    # build a dependency tree
    load_project(ctx)

    # start building
   
main()
