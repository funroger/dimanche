#!/usr/bin/env python3

from di_build_settings import BuildSettings
from di_log import Log, VERBOSITY
from di_project import Project
import multiprocessing
import threading


class BuildFusionCore:
    def __init__(self, build_settings: BuildSettings,
        project_graph: Project, log: Log):
        self.build_settings = build_settings
        self.project_graph = project_graph
        self.log = log

    def build(self):
        # create an action graph

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

    def __thread_proc(self, thread_id: int):
        job = self.__get_job()
        while job:
            self.__complete_job(job)
            job.do()
            print("[%s] %s" % (str(thread_id), job.desc()))
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
        if 12 > self.calls:
            if 0 == self.calls % 3:
                job = self.__Job()
            elif 1 == self.calls % 3:
                job = self.__Job_c()
            elif 2 == self.calls % 3:
                job = self.__Job_cpp()
            self.calls += 1

        self.lock.release()
        return job
