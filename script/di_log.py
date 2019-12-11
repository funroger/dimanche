#!/usr/bin/env python3

# logger uses configuration context as first variable.
# "log_level" variable sets the detail level of the log.

import abc
import di_format
import di_platform
import threading

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


def string_to_verbosity(string) -> VERBOSITY:
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


def verbosity_to_string(verbosity: VERBOSITY) -> str:
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


class Log(abc.ABC):
    def __init__(self):
        pass
        self.__echo = None


    @abc.abstractmethod
    def __repr__(self) -> str:
        return "Log{}"


    def Log(self, level: VERBOSITY, message) -> None:
        if level >= self.verbosity:
            error_message = message
            if VERBOSITY.WARNING == level:
                error_message = di_format.WARNING + ": " + message
            elif VERBOSITY.ERROR == level:
                error_message = di_format.ERROR + ": " + message
            elif VERBOSITY.MESSAGE == level:
                error_message = di_format.COLOR.LIGHT_WHITE + message + di_format.COLOR.NONE
            self.Print(str(error_message))
        if self.__echo: self.__echo.Log(level, message)


    @abc.abstractmethod
    def Print(self, message: str) -> None:
        pass


    def SetEcho(self, echo: Log) -> None:
        self.__echo = echo


class LogPrint(Log):
    def __init__(self,
        verbosity: VERBOSITY = VERBOSITY.DEFAULT,
        echo = None):

        self.verbosity = verbosity
        super(LogPrint, self).SetEcho(echo)

        self.lock = threading.Lock()


    def __repr__(self) -> str:
        return "LogPrint{verbosity=%s}" % verbosity_to_string(self.verbosity)


    def Print(self, message: str) -> None:
        with self.lock:
            print(message)


class LogFile(Log):
    def __init__(self,
        file_path: str,
        verbosity: VERBOSITY = VERBOSITY.DEFAULT,
        echo = None):

        self.file = open(di_platform.expand_path(file_path), "w")
        self.verbosity = verbosity
        super(LogFile, self).SetEcho(echo)

        self.lock = threading.Lock()


    def __repr__(self) -> str:
        return "LogFile{verbosity=%s}" % verbosity_to_string(self.verbosity)


    def Print(self, message: str) -> None:
        with self.lock:
            self.file.write(message + "\n")
