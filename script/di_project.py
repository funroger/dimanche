#!/usr/bin/env python3

import di_log
import di_platform
import json
import os
import re


def update(obj, variables : dict):
    print("-- update str ---")
    print(obj)
    for key, value in variables.items():
        if obj == key:
            print("found %s -> %s" % (key, value))
            return value
    return obj


def update_list(obj : list, variables : dict):
    print("-- update list ---")
    print(obj)
    for x in range(len(obj)):
        value = obj[x]
        if dict == type(value):
            update_dict(value, variables)
        elif list == type(value):
            update_list(value, variables)
        elif str == type(value):
            obj[x] = update(value, variables)


def update_dict(obj : dict, variables : dict):
    print("-- update dict ---")
    print(obj)
    for key, value in obj.items():
        if dict == type(value):
            update_dict(value, variables)
        elif list == type(value):
            update_list(value, variables)
        elif str == type(value):
            obj[key] = update(value, variables)


class Project:
    def __init__(self, project_file_path : str, project_name : str, log : di_log.Log):

        if not os.path.exists(project_file_path):
            di_platform.exit_on_error("can't find the project file at '%s'" % \
                project_file_path, log, __file__)

        self.path = project_file_path
        project_file = open(self.path)
        projects = json.load(project_file)

        # find the project
        for project in projects["projects"]:
            if project_name == project["name"]:
                self.properties = project
                break

        if not self.properties:
            di_platform.exit_on_error("can't find '%s' project in the file '%s'" % \
                (project_name, project_file_path), log, __file__)

        # parse variables
        OS_NAME = di_platform.os_name()
        variables = {}
        for key, value in self.properties["variables"].items():
            if re.match(key, OS_NAME):
                variables = dict(variables, **value)

        # update values
        update_dict(self.properties["build"], variables)

        print(self.properties)

    # properties
    path = ""
    properties = {}


def load_project(project_file_path : str, project_name : str, log : di_log.Log):
    project = Project(project_file_path, project_name, log)
    return project
