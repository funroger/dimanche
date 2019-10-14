#!/usr/bin/env python3

# logger uses configuration context as first variable.
# "log_level" variable sets the detail level of the log.

import colorama
import di_format
import di_platform


class VERBOSITY:
    INFO = 0
    WARNING = 1
    ERROR = 2
    MESSAGE = 3
    SILENT = 1024

    DEFAULT = ERROR


def string_to_verbosity(string):
    verbosity = VERBOSITY.DEFAULT
    if "info" == string:
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
    if VERBOSITY.INFO == verbosity:
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
