#!/usr/bin/env python3

from di_log import Log, VERBOSITY
import di_platform
from di_project_settings import *
import glob
import os
import re


class Project:
    def __init__(self, project_settings: ProjectSettings, log: Log):

        self.settings = project_settings
        self.dependants = set()
        self.dependencies = set()

        log.Log(VERBOSITY.INFO, ("project '%s' created") % self.settings.Name())
        log.Log(VERBOSITY.MAX, str(self.settings))

    def __repr__(self) -> str:
        r = "Project '%s' {" % self.settings.Name()
        r += "'path' = '%s',\n" % self.settings.Path()
        r += "dependencies ["
        for dependency in self.dependencies:
            r += " " + dependency.settings.Name()
        r += "]}"
        return r

    def Dependants(self) -> set:
        return self.dependants

    def Dependencies(self) -> set:
        return self.dependencies

    def Dir(self) -> str:
        return get_project_dir(self.settings.Path())

    def PrepareForBuild(self) -> None:
        project_dir = self.Dir()
        # expand include & source lists
        _expand_pathes(self.settings, project_dir, "include", self.log)
        _expand_pathes(self.settings, project_dir, "source", self.log)

    def SetDependant(self, dependant) -> None:
        if not dependant in self.dependants:
            self.dependants.add(dependant)

    def Settings(self) -> ProjectSettings:
        return self.settings.Get()


def load_project_graph(project_path: str,
    host: str,
    target: str,
    log: Log = None) -> Project:

    project_settings_factory = ProjectSettingsFactory(host, target, log)
    project_cache = {}

    project_graph = _create_project_graph(project_path, project_settings_factory,
        project_cache, log)

    # check the project graph
    _check_for_cycles(project_graph, log)
    log.Log(VERBOSITY.INFO, "project graph checking OK")

    return project_graph


def _create_project_graph(project_path: str,
    project_settings_factory: ProjectSettingsFactory,
    project_cache: dict,
    log: Log) -> Project:

    project_dir = get_project_dir(project_path)
    project_name = get_project_name(project_path)

    if project_path in project_cache:
        log.Log(VERBOSITY.INFO, "project '%s' loaded from the cache" %
            project_name)
        return project_cache[project_path]

    project_settings = project_settings_factory.LoadProject(project_path)
    project = Project(project_settings, log)

    dependencies = set()
    _parse_dependency_settings(dependencies, project.Settings())
    if dependencies:
        log.Log(VERBOSITY.INFO, "loading project '%s' dependencies" % project_name)

        project_file_path = get_project_file_path(project_path)

        idx = 0
        for dependency in dependencies:
            log.Log(VERBOSITY.INFO, "%s:% 2d: %s" % (project_name, idx, dependency))
            idx += 1

            dependency_path = get_relative_project_path(project_path, dependency)
            dependency_name = get_project_name(dependency_path)

            # check for dependency duplicates,
            duplicate = False
            for item in project.dependencies:
                if item.path == dependency_path:
                    duplicate = True
                    log.Log(VERBOSITY.WARNING, "skip project '%s' - it already loaded" %
                        dependency_name)
                    break
            # if not then load the dependency project
            if False == duplicate:
                project_dependency = _create_project_graph(dependency_path,
                    project_settings_factory, project_cache, log)
                project_dependency.SetDependant(project)
                project.dependencies.add(project_dependency)

    return project


def _check_for_cycles(project: Project, log: Log) -> None:

    if hasattr(project, 'visited'):
        di_platform.exit_on_error("a cycle detected in the project graph", __file__, log)

    project.visited = 1
    for dependency in project.dependencies:
        _check_for_cycles(dependency, log)
    del project.visited


def _expand_pathes(settings, project_dir: str, file_type: str, log: Log = None) -> None:
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
                            if log: log.Log(VERBOSITY.WARNING, "nothing found at %s" % path)
                        files_list = files_list + sub_list
                    settings[key] = files_list
                    continue
                item = value
            if dict == type(item):
                _expand_pathes(item, project_dir, file_type, log)
            elif list == type(item):
                _expand_pathes(item, project_dir, file_type, log)


def _parse_dependency_settings(dependencies: set, settings) -> None:
    if str == type(settings):
        dependencies.add(settings)
    elif list == type(settings):
        for item in settings:
            _parse_dependency_settings(dependencies, item)
    elif dict == type(settings):
        if KEY.DEPENDENCIES in settings:
            _parse_dependency_settings(dependencies, settings[KEY.DEPENDENCIES])
