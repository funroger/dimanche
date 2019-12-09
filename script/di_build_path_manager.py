#!/usr/bin/env python3

import di_build_settings
import di_log
import di_platform
import os
import sys


class BuildPathManager:
    def __init__(self, output_root: str, log: di_log.Log = None):
        self.log = log
        self.output_root = di_platform.expand_path(output_root)

    def CreatePath(self, path: str) -> str:
        path = di_platform.expand_path(path)

        # create a path
        output_path = self.output_root
        path_parts = os.path.normpath(path).split(os.sep)
        for item in path_parts:
            output_path = os.path.join(output_path, item)

        # make sure that the folder is exist
        output_dir = output_path
        if os.path.isfile(path):
            output_dir = os.path.dirname(output_path)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        return output_path
