#!/usr/bin/env python3

import di_log
import di_platform
import json
import os
import re


def update(obj, variables: dict, log: di_log.Log = None):
    #print("-- update str ---")
    #print(obj)
    for key, value in variables.items():
        if obj == key:
            if not None == log:
                log.log(di_log.VERBOSITY.INFO, "replace %s -> %s" % (key, value))
            return value
    return obj


def update_list(obj: list, variables: dict, log: di_log.Log = None):
    #print("-- update list ---")
    #print(obj)
    for x in range(len(obj)):
        value = obj[x]
        if dict == type(value):
            update_dict(value, variables, log)
        elif list == type(value):
            update_list(value, variables, log)
        elif str == type(value):
            value = update(value, variables, log)
            if list == type(value):
                obj.pop(x)
                obj.extend(value)
            else:
                obj[x] = value



def update_dict(obj: dict, VARIABLES: dict, log: di_log.Log = None):
    #print("-- update dict ---")
    #print(obj)
    variables = VARIABLES.copy()
    for key, value in obj.items():
        if "variables" == key:
            variables = parse_variables(variables, value)
        elif dict == type(value):
            update_dict(value, variables, log)
        elif list == type(value):
            update_list(value, variables, log)
        elif str == type(value):
            obj[key] = update(value, variables, log)


class Project:
    def __init__(self, project_path: str, log: di_log.Log):

        project_file_path, project_name = split_project_path(project_path)

        if not os.path.exists(project_file_path):
            di_platform.exit_on_error("can't find the project file at '%s'" % \
                project_file_path, log, __file__)

        self.path = project_file_path
        project_file = open(self.path)
        projects = json.load(project_file)

        GLOBAL_VARIABLES = {}
        # update values
        update_dict(projects, GLOBAL_VARIABLES, log)

        # find the project
        for project in projects["projects"]:
            # update variables
            variables = parse_variables(GLOBAL_VARIABLES, project["variables"])

            # update values
            update_dict(project, variables, log)

            if project_name == project["name"]:
                self.properties = project
                break

        if not self.properties:
            di_platform.exit_on_error("can't find '%s' project in the file '%s'" % \
                (project_name, project_file_path), log, __file__)

        log.log(di_log.VERBOSITY.INFO, self.properties)

    # properties
    path = ""
    properties = {}


def load_project(project_path: str, log: di_log.Log):
    project = Project(project_path, log)
    return project


def parse_variables(existing_variables: dict, more_variables: dict):
    OS_NAME = di_platform.os_name();
    variables = existing_variables.copy()

    for key, value in more_variables.items():
        if re.match(key, OS_NAME):
            variables = dict(variables, **value)
    return variables


def split_project_path(project_path: str):
    return project_path.split("::", 1)
