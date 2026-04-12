#!/usr/bin/env python3
import argparse
from ctypes import *
import os
import sys

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

  parser.add_argument('-v', '--version',
                      action='store_true')  # on/off flag

  args = parser.parse_args()
  if sys.platform == 'win32':
    dll_name = './YASS-C.dll'
  elif sys.platform == 'darwin':
    dll_name = './libYASS-C.dylib'
  else:
    dll_name = './libYASS-C.so'

  dll = cdll.LoadLibrary(dll_name)
  dll.yass_app_name.restype = c_char_p
  dll.yass_app_version_str.restype = c_char_p
  if args.version:
    print_version(dll)
    return
  # TODO: TO BE IMPLEMENTED
  print('Hello, World\nvia YASS-C binding')

if __name__ == '__main__':
  main()
