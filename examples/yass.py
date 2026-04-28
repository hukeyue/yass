#!/usr/bin/env python3
import argparse
from ctypes import *
import os
import signal
import sys
import threading

class YassClient:
  def __init__(self, dll):
    print('ctor')
    self.dll = dll
    self.instance = self.dll.yass_client_instance_create()
    ret = self.dll.yass_client_instance_init(self.instance)
    if ret != 0:
      raise Exception(self.get_last_error())

  def add_server(self, server_tag, proxy_uri, listen_uri):
    print('add_server')
    port = c_uint16()
    ret = self.dll.yass_client_instance_add_server_uri(self.instance, c_int64(server_tag), proxy_uri.encode('utf-8'), listen_uri.encode('utf-8'), pointer(port))
    if ret != 0:
      raise Exception(self.get_last_error())
    return port.value

  def run(self):
    print('run')
    signal.signal(signal.SIGINT, lambda sig, frame: self.stop())
    signal.signal(signal.SIGTERM, lambda sig, frame: self.stop())
    # On Windows, pressing Ctrl+C triggers a new thread from the OS
    # that eventually tries to notify Python's main thread.
    # However, if the main thread is stuck in a synchronous join() or I/O operation,
    # it cannot process that notification until the blocking call returns.
    # This differs from Unix-like systems, where the signal interrupts the system call directly.
    if sys.platform == 'win32':
      signal.signal(signal.SIGINT, signal.SIG_DFL)
      signal.signal(signal.SIGTERM, signal.SIG_DFL)
    self.bg_thread = threading.Thread(target=lambda: self._run())
    self.bg_thread.start()
    self.bg_thread.join()

  def _run(self):
    print('run in background thread')
    ret = self.dll.yass_client_instance_run(self.instance)
    if ret != 0:
      raise Exception(self.get_last_error())

  def stop(self):
    print('stop')
    ret = self.dll.yass_client_instance_cancel(self.instance)
    if ret != 0:
      raise Exception(self.get_last_error())

  def get_last_error(self):
    return self.dll.yass_client_instance_get_last_error_str(self.instance).decode('utf-8')

  def __del__(self):
    self.dll.yass_client_instance_destroy(self.instance)
    print('dtor')

def get_dll():
  if sys.platform == 'win32':
    dll_name = './YASS-C.dll'
  elif sys.platform == 'darwin':
    dll_name = './libYASS-C.dylib'
  else:
    dll_name = './libYASS-C.so'

  dll = cdll.LoadLibrary(dll_name)
  dll.yass_app_name.restype = c_char_p #version.h
  dll.yass_app_version_str.restype = c_char_p #version.h
  dll.yass_client_instance_create.restype = c_void_p #client.h
  dll.yass_client_instance_destroy.argtypes = [c_void_p] #client.h
  dll.yass_client_instance_init.argtypes = [c_void_p] #client.h
  dll.yass_client_instance_init.restype = c_int #client.h
  dll.yass_client_instance_add_server_uri.argtypes = [c_void_p, c_int64, c_char_p, c_char_p, POINTER(c_uint16)] #client.h
  dll.yass_client_instance_add_server_uri.restype = c_int #client.h
  dll.yass_client_instance_run.argtypes = [c_void_p] #client.h
  dll.yass_client_instance_run.restype = c_int #client.h
  dll.yass_client_instance_cancel.argtypes = [c_void_p] #client.h
  dll.yass_client_instance_cancel.restype = c_int #client.h
  dll.yass_client_instance_get_last_error_str.restype = c_char_p #client.h
  return dll

def print_version(dll):
  name_str = dll.yass_app_name().decode()
  version_str = dll.yass_app_version_str().decode()
  print(f'{name_str} {version_str}\nvia YASS-C binding')

def main():
  os.chdir(os.path.dirname(os.path.abspath(__file__)))
  parser = argparse.ArgumentParser(
                      prog='yass',
                      description='Example of how to use YASS-C dylib',
                      epilog='NASTY (plugin) client written in C++.')

  parser.add_argument('-v', '--version', action='store_true')  # on/off flag
  parser.add_argument('--tag', nargs='+', default=[0])
  parser.add_argument('--proxy', nargs='+', default=['https://username:password@http2.github.io'])
  parser.add_argument('--listen', nargs='+', default=['auto://127.0.0.1:1080'])

  args = parser.parse_args()
  dll = get_dll()
  if args.version:
    print_version(dll)
    return
  print('Hello, World\nvia YASS-C binding')
  client = YassClient(dll)
  if len(args.tag) != len(args.listen) or len(args.proxy) != len(args.listen):
    raise Exception('--proxy and --listen pairs doesn\'t match')
  for i in range(len(args.tag)):
    port = client.add_server(int(args.tag[i]), args.proxy[i], args.listen[i])
    print(f'tag {args.tag[i]} listen at {args.listen[i]} port {port}')
  client.run()

if __name__ == '__main__':
  main()
