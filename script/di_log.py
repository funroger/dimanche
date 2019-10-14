#!/usr/bin/env python3

# logger uses configuration context as first variable.
# "log_level" variable sets the detail level of the log.

import di_format
import di_platform

# platform specific
if "windows" == di_platform.os_name():
    import colorama
    colorama.init()


class VERBOSITY:
    MAX = 0
    INFO = 1
    WARNING = 2
    ERROR = 3
    MESSAGE = 4
    SILENT = 1024

    DEFAULT = ERROR


def string_to_verbosity(string):
    verbosity = VERBOSITY.DEFAULT
    if "max" == string:
        verbosity = VERBOSITY.MAX
    elif "info" == string:
        verbosity = VERBOSITY.INFO
    elif "warning" == string:
        verbosity = VERBOSITY.WARNING
    elif "error" == string:
        verbosity = VERBOSITY.ERROR
    elif "message" == string:
        verbosity = VERBOSITY.MESSAGE
    elif "silent" == string:
        verbosity = VERBOSITY.SILENT
    return verbosity


def verbosity_to_string(verbosity):
    string = "UNKNOWN"
    if VERBOSITY.MAX == verbosity:
        string = "max"
    elif VERBOSITY.INFO == verbosity:
        string = "info"
    elif VERBOSITY.WARNING == verbosity:
        string = "warning"
    elif VERBOSITY.ERROR == verbosity:
        string = "error"
    elif VERBOSITY.MESSAGE == verbosity:
        string = "message"
    elif VERBOSITY.SILENT == verbosity:
        string = "silent"
    return string


class Log:
    def __init__(self, verbosity = VERBOSITY.DEFAULT):
        self.verbosity = verbosity
        if "windows" == di_platform.os_name():
            colorama.init()

    def __repr__(self):
        return "Log{verbosity=%s}" % verbosity_to_string(self.verbosity)

    def log(self, level, message):
        if level >= self.verbosity:
            error_message = message
            if VERBOSITY.WARNING == level:
                error_message = di_format.WARNING + ": " + message
            elif VERBOSITY.ERROR == level:
                error_message = di_format.ERROR + ": " + message
            elif VERBOSITY.MESSAGE == level:
                error_message = di_format.COLOR.LIGHT_WHITE + message + di_format.COLOR.NONE
            print(error_message)

    # properties
    verbosity = VERBOSITY.DEFAULT
