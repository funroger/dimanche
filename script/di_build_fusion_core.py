#!/usr/bin/env python3

from di_build_settings import BuildSettings
from di_log import Log, VERBOSITY
import di_platform
from di_project import Project, get_project_dir
import multiprocessing
import os
import threading


class BuildFusionCore:
    def __init__(self, project_graph: Project,
        output_root: str, target_os: str, target_platform: str, target_config: str,
        build_settings: BuildSettings, log: Log):
        self.project_graph = project_graph
        # create an output root
        path = os.path.join(output_root, target_os, target_platform, target_config)
        self.output_root = di_platform.expand_path(path)
        self.build_settings = build_settings
        self.log = log
        self.action_list = []

        self.__create_path(self.output_root)


    def build(self):
        # create an action list
        self.__create_action_list()

        # spawn threads
        self.calls = 0
        num_cpus = multiprocessing.cpu_count()
        self.lock = multiprocessing.Lock()
        self.log.log(VERBOSITY.INFO, "%s CPU(s) available" % str(num_cpus))
        self.threads = []
        for thread_id in range(1, num_cpus):
            thread = threading.Thread(target = self.__thread_proc, args = (thread_id, ))
            self.threads.append(thread)
            thread.start()

        # do building
        self.__thread_proc(0)

        # wait for other threads
        for thread in self.threads:
            thread.join()

        self.log.log(VERBOSITY.MESSAGE, "BUILD COMPLETE")


    def __create_path(self, path: str):
        if not os.path.exists(path):
            os.makedirs(path)


    def __create_action_list(self):

        project_output_path = self.__get_project_output_path(self.project_graph.path)
        self.__create_path(project_output_path)

        for dependency in self.project_graph.dependencies:
            self.action_list.append(dependency)
        self.action_list.append(self.project_graph)


    def __get_project_output_path(self, project_path: str):
        project_output_path = self.output_root
        path_parts = os.path.normpath(get_project_dir(project_path)).split(os.sep)
        for item in path_parts:
            project_output_path = os.path.join(project_output_path, item)
        return project_output_path


    def __thread_proc(self, thread_id: int):
        job = self.__get_job()
        while job:
            job.do()
            self.__complete_job(job)
            job = self.__get_job()
        print("Thread %s exited" % str(thread_id))


    class __Job:
        def __init__(self):
            a = 0

        def do(self):
            a = 0

        def desc(self):
            return "Just a job"


    class __Job_c:
        def __init__(self):
            a = 0

        def do(self):
            a = 0

        def desc(self):
            return "C job"


    class __Job_cpp:
        def __init__(self):
            a = 0

        def do(self):
            a = 0

        def desc(self):
            return "CPP job"


    def __complete_job(self, job):
        self.lock.acquire()

        self.lock.release()


    def __get_job(self):
        self.lock.acquire()

        job = None

        self.lock.release()
        return job
