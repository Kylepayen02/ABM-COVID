# Temporary script for compiling the tests

import subprocess, glob, os

#
# Input 
#

# Path to the main directory
path = '../../src/'
# Compiler options
cx = 'g++'
std = '-std=c++11'
opt = '-O3'
# Common source files
src_files = path + 'abm.cpp' 
src_files += ' ' + path + 'agent.cpp' 
src_files += ' ' + path + 'infection.cpp'
src_files += ' ' + path + 'utils.cpp'
src_files += ' ' + path + 'places/place.cpp'
src_files += ' ' + path + 'places/household.cpp'
src_files += ' ' + path + 'io_operations/FileHandler.cpp'

#
# Tests
#

# Name of the executable
exe_name = 'demo'
# Files needed only for this build
spec_files = 'demo_run.cpp '
compile_com = ' '.join([cx, std, opt, '-o', exe_name, spec_files, src_files])
subprocess.call([compile_com], shell=True)


