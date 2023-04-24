import ctypes
import subprocess
import os
import threading
import time
import socket
import sys

# TODO: Will I need absolute path here?
OBJ_DIR = 'objects'

print("\n\n###\n Hello world! Starting Python application ... \n###\n\n")

class proc_start_payload(ctypes.Structure):
    _fields_ = [
        ('exe_path',    ctypes.c_char * 100),
        ('pid',         ctypes.c_int),
        ('ppid',        ctypes.c_int),
    ]

def start_handling_events_thread():
    print("Starting to handle events ...")
    workerThread = threading.Thread(target=eps_lib.start_handling_events, args=())
    workerThread.start()

def start_metrics_thread():
    print("Starting metrics thread ...")
    workerThread = threading.Thread(target=eps_lib.report_metrics, args=())
    workerThread.start()

def safe_shutdown():
    eps_lib.shutdown()
    time.sleep(1)
    sys.exit(0)

eps_lib = ctypes.CDLL(f"{OBJ_DIR}/eps_lib.so")

start_handling_events_thread()
start_metrics_thread()

read_event_from_q = eps_lib.read_from_global_struct_queue
read_event_from_q.restype = proc_start_payload

cmd = input("'r' to read msg from queue, 'q' to quit \n")
while True:
    if cmd == 'q':
        safe_shutdown()
    if cmd == 'r':
        try:
            evt = read_event_from_q()
            # TODO: How can I prevent errors when q is empty? besides just a try/except
            print(f"Recv'd {evt.exe_path.decode('utf-8')} from que with PID {evt.pid}")
        except Exception:
            safe_shutdown()
    cmd = input("")

