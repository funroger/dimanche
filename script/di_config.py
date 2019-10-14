#!/usr/bin/env python3

import di_log
import di_platform
import json
import os


class Config:
    def __init__(self, config_file_path: str, log: di_log):

        if not os.path.exists(config_file_path):
            di_platform.exit_on_error("can't find the config file at '%s'" % \
                config_file_path, log, __file__)

        self.path = config_file_path
        config_file = open(self.path)
        self.properties = json.load(config_file)

        for key, value in self.properties.items():
            if not "defaults" == key:
                for tool_type, tools in value.items():
                    for tool in tools:
                        tool_name = tool["name"]
                        tool_path = tool["path"]
                        if not os.path.exists(tool_path):
                            log.log(di_log.VERBOSITY.WARNING,
                                "can't find the tool '%s' at '%s'" % \
                                (tool_name + ":" + tool_type, tool_path))
                        log.log(di_log.VERBOSITY.INFO, tool_name + ":" + tool_type + ": " + tool_path)

    def __repr__(self):
        rep = "Config{"
        rep += "path='%s'" % self.path
        for k, v in self.properties.items():
            rep += ", %s='%s'" % (k, v)
        rep += "}"
        return rep

    # properties
    path = ""
    properties = {}


def load_config(config_file_path: str, log: di_log.Log):
    config = Config(config_file_path, log)
    return config
