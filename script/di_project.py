#!/usr/bin/env python3

from di_log import Log, VERBOSITY
import di_platform
import glob
import json
import os
import re


class Project:
    def __init__(self, project_path: str, project_settings: dict, log: Log):

        self.path = project_path
        self.settings = project_settings
        self.dependants = set()
        self.dependencies = set()

        log.log(VERBOSITY.INFO, ("project '%s' created") % get_project_name(project_path))
        log.log(VERBOSITY.MAX, self.settings)


    def __repr__(self):
        r = "Project '%s' {\n" % self.settings["name"]
        r += "    path = %s\n" % self.path
        r += "    dependencies ["
        for dependency in self.dependencies:
            r += " " + dependency.settings["name"]
        r += "]\n}"
        return r


    def dir(self):
        return get_project_dir(self.path)


    def dependants(self):
        return self.dependants


    def dependencies(self):
        return self.dependencies


    def prepare_for_build(self):
        project_dir = self.dir()
        # expand include & source lists
        __expand_pathes(self.settings, project_dir, "include", self.log)
        __expand_pathes(self.settings, project_dir, "source", self.log)


    def set_dependant(self, dependant):
        if not dependant in self.dependants:
            self.dependants.add(dependant)


def get_project_dir(project_path: str):
    return os.path.dirname(get_project_file_path(project_path))


def get_project_file_path(project_path: str):
    return project_path.split("::", 1)[0]


def get_project_name(project_path: str):
    result = project_path.split("::", 1)
    return result[1] if 2 <= len(result) else ""


def load_project_graph(project_path: str, target: str, log: Log):

    project_files_cache = {}
    project_cache = {}

    project_graph = __build_project_graph(project_path, target,
        project_files_cache, project_cache, log)

    # check the project graph
    __check_for_cycles(project_graph, log)
    log.log(VERBOSITY.INFO, "project graph checking OK")

    return project_graph


def __build_project_graph(project_path: str, target: str,
    project_files_cache: dict, project_cache: dict, log: Log):

    project_dir = get_project_dir(project_path)
    project_name = get_project_name(project_path)

    if project_path in project_cache:
        log.log(VERBOSITY.INFO, "project '%s' loaded from the cache" %
            project_name)
        return project_cache[project_path]

    project = __load_project(project_path, target,
        project_files_cache, project_cache, log)

    dependencies = project.settings.get("dependency", [])
    if dependencies:
        log.log(VERBOSITY.INFO, "loading project '%s' dependencies" % project_name)

        project_file_path = get_project_file_path(project_path)

        idx = 0
        for dependency in dependencies:
            log.log(VERBOSITY.INFO, "%s:% 2d: %s" % (project_name, idx, dependency))
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
                dependency_path = di_platform.expand_path(dependency_path)
                dependency_path += "::" + dependency_name

            # check for dependency duplicates,
            duplicate = False
            for item in project.dependencies:
                if item.path == dependency_path:
                    duplicate = True
                    log.log(VERBOSITY.WARNING, "skip project '%s' - it already loaded" %
                        dependency_name)
                    break
            # if not then load the dependency project
            if False == duplicate:
                project_dependency = __build_project_graph(dependency_path,
                    target, project_files_cache, project_cache, log)
                project_dependency.set_dependant(project)
                project.dependencies.add(project_dependency)

    return project


def __check_for_cycles(project: Project, log: Log):

    if hasattr(project, 'visited'):
        di_platform.exit_on_error("a cycle detected in the project graph", log, __file__)

    project.visited = 1
    for dependency in project.dependencies:
        __check_for_cycles(dependency, log)
    del project.visited


def __load_project(project_path: str, target: str,
    project_files_cache: dict, project_cache: dict, log: Log):

    project_file_path = get_project_file_path(project_path)
    project_name = get_project_name(project_path)

    if not os.path.exists(project_file_path) or not os.path.isfile(project_file_path):
        di_platform.exit_on_error("can't find the project file at '%s'" % \
            project_file_path, log, __file__)

    project_file = {}

    if not project_file_path in project_files_cache:
        file = open(project_file_path)
        project_file = json.load(file)

        # filter platform & host
        host = di_platform.os_name()
        __filter(project_file, target, host)

        # update values
        GLOBAL_VARIABLES = {}
        __expand_variables_dict(project_file, GLOBAL_VARIABLES, log)

        project_files_cache[project_file_path] = project_file
        log.log(VERBOSITY.INFO, "project file '%s' loaded from the file" %
            project_file_path)
    else:
        project_file = project_files_cache[project_file_path]
        log.log(VERBOSITY.INFO, "project file '%s' loaded from the cache" %
            project_file_path)

    # find the project
    project = {}
    if "projects" in project_file:
        for name in project_file["projects"]:
            if project_name == name:
                project_settings = project_file["projects"][name]
                project = Project(project_path, project_settings, log)
                project_cache[project_path] = project
                break

    if not project:
        di_platform.exit_on_error("can't find project '%s' in the file '%s'" % \
            (project_name, project_file_path), log, __file__)

    return project


def __expand_pathes(settings, project_dir: str, file_type: str, log: Log = None):
    if dict == type(settings) or list == type(settings):
        for item in settings:
            if dict == type(settings):
                key = item
                value = settings[key]
                if file_type == key and list == type(value):
                    files_list = []
                    for file_item in value:
                        path = os.path.expanduser(file_item)
                        path = os.path.join(project_dir, path)
                        path = di_platform.expand_path(path)
                        sub_list = []
                        for file_path in glob.glob(path, recursive = True):
                            sub_list.append(file_path)
                        if 0 == len(sub_list):
                            if log: log.log(VERBOSITY.WARNING, "nothing found at %s" % path)
                        files_list = files_list + sub_list
                    settings[key] = files_list
                    continue
                item = value
            if dict == type(item):
                __expand_pathes(item, project_dir, file_type, log)
            elif list == type(item):
                __expand_pathes(item, project_dir, file_type, log)


def __expand_variables(obj, variables: dict, log: Log = None):
    if obj in variables:
        if log: log.log(VERBOSITY.MAX, "replace %s -> %s" % (obj, variables[obj]))
        return variables[obj]
    else:
        return obj


def __expand_variables_dict(obj: dict, VARIABLES: dict, log: Log = None):
    variables = VARIABLES
    if "variables" in obj:
        variables = __update_variables(variables, obj["variables"])
        obj.pop("variables")

    for key, value in obj.items():
        if dict == type(value):
            __expand_variables_dict(value, variables, log)
        elif list == type(value):
            __expand_variables_list(value, variables, log)
        elif str == type(value):
            obj[key] = __expand_variables(value, variables, log)


def __expand_variables_list(obj: list, variables: dict, log: Log = None):
    for x in range(len(obj)):
        value = obj[x]
        if dict == type(value):
            __expand_variables_dict(value, variables, log)
        elif list == type(value):
            __expand_variables_list(value, variables, log)
        elif str == type(value):
            value = __expand_variables(value, variables, log)
            if list == type(value):
                obj.pop(x)
                obj.extend(value)
            else:
                obj[x] = value


def __filter(settings, target: str, host: str):
    if dict == type(settings):
        if False == __match_host_and_target(settings, host, target):
            settings.clear()
        else:
            if "host" in settings: settings.pop("host")
            if "target" in settings: settings.pop("target")
            for key in settings: __filter(settings[key], target, host)
            empty_keys = [key for key, value in settings.items() if not value]
            for key in empty_keys: del settings[key]
    elif list == type(settings):
        for item in settings: __filter(item, target, host)
        while {} in settings: settings.remove({})
        while [] in settings: settings.remove([])


def __match_host_and_target(settings: dict, host, target):
    if ("host" in settings and not re.match(settings["host"], host) or \
       ("target" in settings and not re.match(settings["target"], target))):
        return False
    else:
        return True


def __update_variables(existing_variables: dict, more_variables: dict):
    host = di_platform.os_name()
    variables = existing_variables.copy()

    for item in more_variables:
        variables = dict(variables, **item)
    return variables
