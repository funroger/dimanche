#!/usr/bin/env python3

import di_log
import di_platform
import json
import os
import re


class Project:
    def __init__(self, project_path: str, project_settings: dict, log: di_log.Log):

        self.path = project_path
        self.settings = project_settings
        self.dependencies = []
        self.visited = 0

        log.log(di_log.VERBOSITY.INFO, ("project '%s' loaded") % get_project_name(project_path))
        log.log(di_log.VERBOSITY.MAX, self.settings)

    def __repr__(self):
        r = "Project '%s' {\n" % self.settings["name"]
        r += "    path = %s\n" % self.path
        r += "    dependencies ["
        for dependency in self.dependencies:
            r += " " + dependency.settings["name"]
        r += "]\n}"
        return r


def get_project_dir(project_path: str):
    return os.path.dirname(get_project_file_path(project_path))


def get_project_file_path(project_path: str):
    return project_path.split("::", 1)[0]


def get_project_name(project_path: str):
    return project_path.split("::", 1)[1]


def load_project(project_path: str, log: di_log.Log):

    project_files_cache = {}
    project_cache = {}

    project_graph = __build_project_graph(project_path,
        project_files_cache, project_cache, log)

    # check the project graph
    __check_for_cycles(project_graph, log)
    log.log(di_log.VERBOSITY.INFO, "project graph checking OK")

    return project_graph


def __build_project_graph(project_path: str, project_files_cache: dict,
    project_cache: dict, log: di_log.Log):

    project_name = get_project_name(project_path)

    if project_path in project_cache:
        log.log(di_log.VERBOSITY.INFO, "project '%s' loaded from the cache" %
            project_name)
        return project_cache[project_path]

    project = __load_project(project_path, project_files_cache,
        project_cache, log)

    dependencies = project.settings["dependency"]
    if dependencies:
        log.log(di_log.VERBOSITY.INFO, "loading project '%s' dependencies" % project_name)

        project_dir = get_project_dir(project_path)
        project_file_path = get_project_file_path(project_path)

        idx = 0
        for dependency in dependencies:
            log.log(di_log.VERBOSITY.INFO, "%s:% 2d: %s" % (project_name, idx, dependency))
            idx += 1

            # create the path to dependency
            dependency_path = dependency
            dependency_file_path = get_project_file_path(dependency)
            dependency_name = get_project_name(dependency)

            if "" == dependency_file_path:
                # the same project file
                dependency_path = project_file_path
                dependency_path += "::" + dependency_name
            elif not os.path.isabs(dependency_file_path):
                # a relative path to a project file
                dependency_path = os.path.join(project_dir, dependency_file_path)
                dependency_path = os.path.abspath(dependency_path)
                dependency_path += "::" + dependency_name

            # check for dependency duplicates,
            duplicate = False
            for list_item in project.dependencies:
                if list_item.path == dependency_path:
                    duplicate = True
                    log.log(di_log.VERBOSITY.WARNING, "skip project '%s' - it already loaded" %
                        dependency_name)
                    break
            # if not then load the dependency project
            if False == duplicate:
                project.dependencies.append(__build_project_graph(dependency_path,
                    project_files_cache, project_cache, log))

    return project


def __check_for_cycles(project: Project, log: di_log.Log):

    if not 0 == project.visited:
        di_platform.exit_on_error("a cycle detected in the project graph", log, __file__)

    project.visited = 1
    for dependency in project.dependencies:
        __check_for_cycles(dependency, log)
    project.visited = 0


def __load_project(project_path: str, project_files_cache: dict,
    project_cache: dict, log: di_log.Log):

    project_file_path = get_project_file_path(project_path)
    project_name = get_project_name(project_path)

    if not os.path.exists(project_file_path) or not os.path.isfile(project_file_path):
        di_platform.exit_on_error("can't find the project file at '%s'" % \
            project_file_path, log, __file__)

    project_file = {}

    if not project_file_path in project_files_cache:
        file = open(project_file_path)
        project_file = json.load(file)

        # update values
        GLOBAL_VARIABLES = {}
        __update_dict(project_file, GLOBAL_VARIABLES, log)

        project_files_cache[project_file_path] = project_file
        log.log(di_log.VERBOSITY.INFO, "project file '%s' loaded from the file" %
            project_file_path)
    else:
        project_file = project_files_cache[project_file_path]
        log.log(di_log.VERBOSITY.INFO, "project file '%s' loaded from the cache" %
            project_file_path)

    # find the project
    project = {}
    for project_settings in project_file["projects"]:
        if project_name == project_settings["name"]:
            project = Project(project_path, project_settings, log)
            project_cache[project_path] = project

    if not project:
        di_platform.exit_on_error("can't find project '%s' in the file '%s'" % \
            (project_name, project_file_path), log, __file__)

    return project


def __update(obj, variables: dict, log: di_log.Log = None):
    #print("-- update str ---")
    #print(obj)
    for key, value in variables.items():
        if obj == key:
            if not None == log:
                log.log(di_log.VERBOSITY.MAX, "replace %s -> %s" % (key, value))
            return value
    return obj


def __update_dict(obj: dict, VARIABLES: dict, log: di_log.Log = None):
    #print("-- update dict ---")
    #print(obj)
    variables = VARIABLES.copy()
    for key, value in obj.items():
        if "variables" == key:
            variables = __update_variables(variables, value)
        elif dict == type(value):
            __update_dict(value, variables, log)
        elif list == type(value):
            __update_list(value, variables, log)
        elif str == type(value):
            obj[key] = __update(value, variables, log)


def __update_list(obj: list, variables: dict, log: di_log.Log = None):
    #print("-- update list ---")
    #print(obj)
    for x in range(len(obj)):
        value = obj[x]
        if dict == type(value):
            __update_dict(value, variables, log)
        elif list == type(value):
            __update_list(value, variables, log)
        elif str == type(value):
            value = __update(value, variables, log)
            if list == type(value):
                obj.pop(x)
                obj.extend(value)
            else:
                obj[x] = value


def __update_variables(existing_variables: dict, more_variables: dict):
    OS_NAME = di_platform.os_name()
    variables = existing_variables.copy()

    for key, value in more_variables.items():
        if re.match(key, OS_NAME):
            variables = dict(variables, **value)
    return variables
