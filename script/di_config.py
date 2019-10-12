#!/usr/bin/env python3

import di_log
import di_platform
import json
import os


class Config:
    def __init__(self, config_file_path : str, log : di_log):

        if not os.path.exists(config_file_path):
            di_platform.exit_on_error("can't find the config file at '%s'" % \
                config_file_path, log, __file__)

        self.path = config_file_path
        config_file = open(self.path)
        self.properties = json.load(config_file)

        for tools in self.properties["tools"]:
            for tool in tools["items"]:
                tool_type = tool["type"]
                tool_path = tool["path"]
                if not os.path.exists(tool_path):
                    di_platform.exit_on_error("can't find the tool for '%s' at '%s'" % \
                        (tool_type, tool_path), log, __file__)
        
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


def LoadConfig(config_file_path : str, log : di_log.Log):
    config = Config(config_file_path, log)
    return config
