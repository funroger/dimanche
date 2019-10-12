#!/usr/bin/env python3

# logger uses configuration context as first variable.
# "log_level" variable sets the detail level of the log.

import di_format

INFO=0
WARNING=1
ERROR=2
MESSAGE=3
SILENT=1024

DEFAULT_VERBOSITY=ERROR


def string_to_verbosity(string):
    verbosity = DEFAULT_VERBOSITY
    if "info" == string:
        verbosity = INFO
    elif "warning" == string:
        verbosity = WARNING
    elif "error" == string:
        verbosity = ERROR
    elif "message" == string:
        verbosity = MESSAGE
    elif "silent" == string:
        verbosity = SILENT
    return verbosity


def verbosity_to_string(verbosity):
    string = "UNKNOWN"
    if INFO == verbosity:
        string = "info"
    elif WARNING == verbosity:
        string = "warning"
    elif ERROR == verbosity:
        string = "error"
    elif MESSAGE == verbosity:
        string = "message"
    elif SILENT == verbosity:
        string = "silent"
    return string


class Log:
    def __init__(self, verbosity = DEFAULT_VERBOSITY):
        self.verbosity = verbosity

    def __repr__(self):
        return "Log{verbosity=%s}" % verbosity_to_string(self.verbosity)

    def log(self, level, message):
        if level >= self.verbosity:
            error_message = message
            if WARNING == level:
                error_message = di_format.WARNING + ": " + message
            elif ERROR == level:
                error_message = di_format.ERROR + ": " + message
            elif MESSAGE == level:
                error_message = di_format.LIGHT_WHITE + message + di_format.NC
            print(error_message)

    # properties
    verbosity = DEFAULT_VERBOSITY
