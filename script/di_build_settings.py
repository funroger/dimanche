#!/usr/bin/env python3

import di_log
import di_platform
import json
import os


class BuildSettings:
    def __init__(self, build_settings_file_path: str, log: di_log):

        if not os.path.exists(build_settings_file_path):
            di_platform.exit_on_error("can't find the build settings file at '%s'" % \
                build_settings_file_path, __file__, log)

        self.path = build_settings_file_path
        build_settings_file = open(self.path)
        self.properties = json.load(build_settings_file)

        for key, value in self.properties.items():
            if not "defaults" == key:
                for tool_type, tools in value.items():
                    for tool in tools:
                        tool_name = tool["name"]
                        tool_path = tool["path"]
                        if not os.path.exists(tool_path):
                            log.Log(di_log.VERBOSITY.WARNING,
                                "can't find the tool '%s' at '%s'" % \
                                (tool_name + ":" + tool_type, tool_path))
                        log.Log(di_log.VERBOSITY.MAX, tool_name + ":" + tool_type + ": " + tool_path)

    def __repr__(self) -> str:
        r = "BuildSettings {"
        r += "'path' = '%s',\n" % self.path
        r += str(self.properties)
        r += "}"
        return r


def load_build_settings(build_settings_file_path: str, log: di_log.Log) -> BuildSettings:
    settings = BuildSettings(build_settings_file_path, log)
    return settings
