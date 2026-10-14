#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from multiprocessing import Pool
from multiprocessing import cpu_count
import signal
import osUtils

kilobyte = 1024
stop_loop = 0
cores_on_board = cpu_count()
total_memory_on_board, used_memory_on_board, free_ram_on_board = osUtils.memInfo()

def exit_child(x,y):
    global stop_loop
    stop_loop = 1

def f(x):
    global stop_loop
    block = 0
    memory = {}
    while not stop_loop:
        if block < (200 *1024):
            # set 1KB of memory with ASCII character A
            memory[block] = 'A' * kilobyte
            block = block + 1
        else:
            del memory
            memory = {}
            block = 0
        x * x
    del memory

signal.signal(signal.SIGINT, exit_child)

if __name__ == '__main__':
    print(('-' * 20))
    print('Running load on CPU')
    print(('Utilizing %d cores' % cores_on_board))
    print(('-' * 20))
    pool = Pool(cores_on_board)
    pool.map(f, list(range(cores_on_board)))
