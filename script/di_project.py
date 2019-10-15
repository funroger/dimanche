#!/usr/bin/env python3

import di_log
import di_platform
import json
import os
import re


class Project:
    def __init__(self, project_path: str, project: dict, log: di_log.Log):

        self.path = project_path
        self.properties = project

        log.log(di_log.VERBOSITY.INFO, ("project '%s' loaded") % get_project_name(project_path))
        log.log(di_log.VERBOSITY.MAX, self.properties)

    # properties
    path = ""
    properties = {}
    dependencies = []


def get_project_dir(project_path: str):
    return project_path.split("::", 1)[0].rsplit("/", 1)[0]


def get_project_file_path(project_path: str):
    return project_path.split("::", 1)[0]


def get_project_name(project_path: str):
    return project_path.split("::", 1)[1]


def load_project(project_path: str, log: di_log.Log):

    project_files_cache = {}

    project_tree = __build_project_tree(project_path, project_files_cache, log)

    # check the project tree

    return project_tree


def __build_project_tree(project_path: str, project_files_cache: dict, log: di_log.Log):

    project = __load_project(project_path, project_files_cache, log)

    dependencies = project.properties["dependency"]
    if dependencies:
        log.log(di_log.VERBOSITY.INFO, "loading project dependencies")

        project_dir = get_project_dir(project_path)
        project_file_path = get_project_file_path(project_path)

        for dependency in dependencies:
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
                dependency_path = project_dir
                dependency_path += "/" + dependency_file_path
                dependency_path = os.path.abspath(dependency_path)
                dependency_path += "::" + dependency_name

            project.dependencies.append(__build_project_tree(dependency_path,
                project_files_cache, log))

    return project


def __load_project(project_path: str, project_files_cache: dict, log: di_log.Log):

    project_file_path = get_project_file_path(project_path)
    project_name = get_project_name(project_path)

    if not os.path.exists(project_file_path) or not os.path.isfile(project_file_path):
        di_platform.exit_on_error("can't find the project file at '%s'" % \
            project_file_path, log, __file__)

    projects = {}

    if not project_file_path in project_files_cache:
        project_file = open(project_file_path)
        projects = json.load(project_file)

        # update values
        GLOBAL_VARIABLES = {}
        __update_dict(projects, GLOBAL_VARIABLES, log)

        project_files_cache[project_file_path] = projects
        log.log(di_log.VERBOSITY.INFO, "project '%s' loaded from the file" % project_name)
    else:
        projects = project_files_cache[project_file_path]
        log.log(di_log.VERBOSITY.INFO, "project '%s' loaded from the cache" % project_name)

    # find the project
    for project in projects["projects"]:
        if project_name == project["name"]:
            return Project(project_path, project, log)

    di_platform.exit_on_error("can't find project '%s' in the file '%s'" % \
        (project_name, project_file_path), log, __file__)


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
