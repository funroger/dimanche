#!/usr/bin/env python3

from di_log import Log, VERBOSITY
import di_platform
import json
import os
import re


# declare constant keys
class KEY:
    DEPENDENCIES = "dependencies"
    HOST = "host"
    PROJECTS = "projects"
    TARGET = "target"
    TEST = "test"
    VARIABLES = "variables"


class ProjectSettings:
    def __init__(self, path: str, settings: dict, log: Log):
        self.log = log
        self.path = path
        self.settings = settings


    def __repr__(self) -> str:
        r = "{\n"
        r += self.settings
        r += "\n}"
        return r


    # get a full path to the test project
    def GetTestProjectPath(self) -> str:
        if KEY.TEST in self.settings:
            return get_relative_project_path(self.path, self.settings[KEY_TEST])
        else:
            return ""


    def Name(self) -> str:
        return get_project_name(self.path)


    def Path(self) -> str:
        return self.path


    def Get(self) -> dict:
        return self.settings


class ProjectSettingsFactory:
    def __init__(self, host: str, target: str, log: Log = None):
        self.project_files_cache = dict()
        self.host = host
        self.target = target
        self.log = log

    # load a project settings from a path and return an instance of
    # ProjectSettings class
    def LoadProject(self, project_path: str) -> ProjectSettings:
        project_file_path = get_project_file_path(project_path)
        project_name = get_project_name(project_path)

        if not os.path.exists(project_file_path) or not os.path.isfile(project_file_path):
            di_platform.exit_on_error("can't find the project file at '%s'" % \
                project_file_path, __file__, self.log)

        project_file = {}

        if not project_file_path in self.project_files_cache:
            file = open(project_file_path)
            project_file = json.load(file)

            _filter_host_target(project_file, self.host, self.target)

            GLOBAL_VARIABLES = {}
            _expand_variables_dict(project_file, GLOBAL_VARIABLES, self.log)

            self.project_files_cache[project_file_path] = project_file
            if self.log: self.log.Log(VERBOSITY.INFO, "project file '%s' loaded from the file" %
                project_file_path)
        else:
            project_file = self.project_files_cache[project_file_path]
            if self.log: self.log.Log(VERBOSITY.INFO, "project file '%s' loaded from the cache" %
                project_file_path)

        # find the project
        if (KEY.PROJECTS in project_file) and \
            (project_name in project_file[KEY.PROJECTS]):
                project_settings = ProjectSettings(project_path, 
                    project_file[KEY.PROJECTS][project_name], self.log)
                return project_settings

        di_platform.exit_on_error("can't find project '%s' in the file '%s'" % \
            (project_name, project_file_path), __file__, self.log)


def get_project_dir(project_path: str) -> str:
    return os.path.dirname(get_project_file_path(project_path))


def get_project_file_path(project_path: str) -> str:
    return project_path.split("::", 1)[0]


def get_project_name(project_path: str) -> str:
    result = project_path.split("::", 1)
    return result[1] if 2 <= len(result) else ""


def get_relative_project_path(project_path: str, relative_path: str) -> str:
    project_dir = get_project_dir(project_path)
    project_file_path = get_project_file_path(project_path)

    # create the path to a relative
    relative_file_path = get_project_file_path(relative_path)
    relative_name = get_project_name(relative_path)

    if "" == relative_file_path:
        # the same project file
        relative_path = project_file_path
        relative_path += "::" + relative_name
    elif not os.path.isabs(relative_file_path):
        # a relative path to a project file
        relative_path = os.path.join(project_dir, relative_file_path)
        relative_path = di_platform.expand_path(relative_path)
        relative_path += "::" + relative_name

    return relative_path


def _expand_variables(obj, variables: dict, log: Log = None) -> str:
    # a quick find a variable
    if obj in variables:
        if log: log.Log(VERBOSITY.MAX, "replace %s -> %s" % (obj, variables[obj]))
        return variables[obj]
    # find a partial update of a string
    for variable, value in variables.items():
        if variable in obj:
            updated = obj.replace(variable, value)
            if log: log.Log(VERBOSITY.MAX, "replace %s -> %s" % (obj, updated))
            obj = updated
    return obj


def _expand_variables_dict(obj: dict, VARIABLES: dict, log: Log = None) -> None:
    variables = VARIABLES
    if KEY.VARIABLES in obj:
        variables = _update_variables(variables, obj[KEY.VARIABLES])
        obj.pop(KEY.VARIABLES)

    for key, value in obj.items():
        if dict == type(value):
            _expand_variables_dict(value, variables, log)
        elif list == type(value):
            _expand_variables_list(value, variables, log)
        elif str == type(value):
            obj[key] = _expand_variables(value, variables, log)


def _expand_variables_list(obj: list, variables: dict, log: Log = None) -> None:
    for x in range(len(obj)):
        value = obj[x]
        if dict == type(value):
            _expand_variables_dict(value, variables, log)
        elif list == type(value):
            _expand_variables_list(value, variables, log)
        elif str == type(value):
            value = _expand_variables(value, variables, log)
            if list == type(value):
                obj.pop(x)
                obj.extend(value)
            else:
                obj[x] = value


def _filter_host_target(settings, host: str, target: str) -> None:
    if dict == type(settings):
        if False == _match_host_and_target(settings, host, target):
            settings.clear()
        else:
            if KEY.HOST in settings: settings.pop(KEY.HOST)
            if KEY.TARGET in settings: settings.pop(KEY.TARGET)
            for key in settings: _filter_host_target(settings[key], host, target)
            empty_keys = [key for key, value in settings.items() if not value]
            for key in empty_keys: del settings[key]
    elif list == type(settings):
        for item in settings: _filter_host_target(item, host, target)
        while {} in settings: settings.remove({})
        while [] in settings: settings.remove([])


def _match_host_and_target(settings: dict, host, target) -> bool:
    if ((KEY.HOST in settings and not re.match(settings[KEY.HOST], host)) or \
       (KEY.TARGET in settings and not re.match(settings[KEY.TARGET], target))):
        return False
    else:
        return True


def _update_variables(existing_variables: dict, more_variables: dict) -> dict:
    host = di_platform.os_name()
    variables = existing_variables.copy()

    for item in more_variables:
        variables = dict(variables, **item)
    return variables
