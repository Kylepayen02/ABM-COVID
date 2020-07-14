# ------------------------------------------------------------------
#
#	Tests the input verification functionality for simple py 
#		generation script 	
#
# ------------------------------------------------------------------

import sys
py_path = '../../scripts/'
sys.path.insert(0, py_path)

import copy

import utils as ut
from colors import *
import input_check as ic

#
# Population settings 
#

# Number of agents and initially infected agents
n_agents = 10
n_infected = 3	

# Number of agents in schools and workplaces
n_ags = 3
n_awk = 6

# Number of houses, schools, and workplaces
nh = 5
ns = 2
nw = 6

# Spatial limits, km
sp_lim_x = [10, 150]
sp_lim_y = [100, 120]

#
# Input collection
#

# Data files
agent_fname = 'test_data/sr_agents_data.txt'
houses_fname = 'test_data/sr_houses_data.txt'
schools_fname = 'test_data/sr_schools_data.txt'
workplaces_fname = 'test_data/sr_workplaces_data.txt'

# Load the data as lists of separated strings
agent_data = ic.load_data_for_checking(agent_fname)
houses_data = ic.load_data_for_checking(houses_fname)
schools_data = ic.load_data_for_checking(schools_fname)
workplaces_data = ic.load_data_for_checking(workplaces_fname)

#
# Tests
# 

# # # Test IDs of places
ut.msg('IDs of places - all correct:', BLUE)
ut.test_pass(ic.test_place_IDs(houses_data), 'Houses IDs')
ut.test_pass(ic.test_place_IDs(schools_data), 'Schools IDs')
ut.test_pass(ic.test_place_IDs(workplaces_data), 'Workplace IDs')

# Tests both ID = 0 and not conitnuous
ut.msg('IDs of places - incorrect:', BLUE)
wrong_IDs_houses_data = ic.load_data_for_checking('test_data/houses_wrong_IDs.txt')
ut.test_pass(ic.test_place_IDs(wrong_IDs_houses_data), 'Houses IDs')

# # # Test number of objects
ut.msg('Number of objects - all correct:', BLUE)
ut.test_pass(ic.check_numbers(agent_data, n_agents), 'Agent number')
ut.test_pass(ic.check_numbers(houses_data, nh), 'House number')
ut.test_pass(ic.check_numbers(schools_data, ns), 'School number')
ut.test_pass(ic.check_numbers(workplaces_data, nw), 'Workplace number')

ut.msg('Number of objects - incorrect:', BLUE)
ut.test_pass(ic.check_numbers(workplaces_data, nw-1), 'Workplace number')

# # # Test if no entries are missing from a line/one object information
ut.msg('Number of object entries - all correct:', BLUE)
ut.test_pass(ic.check_entry_number(agent_data, 9), 'Agent entry number')
ut.test_pass(ic.check_entry_number(houses_data, 3), 'House entry number')
ut.test_pass(ic.check_entry_number(schools_data, 3), 'School entry number')
ut.test_pass(ic.check_entry_number(workplaces_data, 3), 'Workplace entry number')

ut.msg('Number of object entries - incorrect:', BLUE)
ut.test_pass(ic.check_entry_number(schools_data, 11), 'School entry number')

# # # Test places for place-specific features
ut.msg('Place features - correct:', BLUE)
ut.test_pass(ic.test_place_properties(houses_data, sp_lim_x, sp_lim_y), 'House properties')
ut.test_pass(ic.test_place_properties(schools_data, sp_lim_x, sp_lim_y), 'Schools properties')
ut.test_pass(ic.test_place_properties(workplaces_data, sp_lim_x, sp_lim_y), 'Workplace properties')

ut.msg('Place features - incorrect:', BLUE)
ut.test_pass(ic.test_place_properties(schools_data, [-10.0, sp_lim_x[0]], sp_lim_y), 'Schools properties')
ut.test_pass(ic.test_place_properties(schools_data, sp_lim_x, [-10.0, sp_lim_y[0]]), 'Schools properties')

# # # Test agents for agent-specific features
ut.msg('Agent features - correct', BLUE)
ut.test_pass(ic.test_agent_properties(agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

ut.msg('Agent features - incorrect', BLUE)

# Wrong school ID
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[2][6] = '120'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# School ID but not student
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[3][0] = '0'
wrong_agent_data[3][6] = '1'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# Wrong work ID
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[2][7] = '120'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# School ID but not student
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[3][1] = '0'
wrong_agent_data[3][7] = '1'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# House ID out of range
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[1][5] = '-120'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[4][5] = '120'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# Agent infection status not 0 or 1
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[1][8] = '-1'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[1][8] = '1000'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# Agent initial coordinates not equal house coordinates
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[1][3] = '-1.0'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[1][4] = '-12.0'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# Agent age is negative
wrong_agent_data = copy.deepcopy(agent_data)
wrong_agent_data[1][2] = '-1'
ut.test_pass(ic.test_agent_properties(wrong_agent_data, houses_data, schools_data, workplaces_data), 'Agent properties')

# # # Tests for total number of infected,  at schools and work
ut.msg('Number of agents in each category - all correct:', BLUE)
ut.test_pass(ic.check_agent_numbers(agent_data, n_infected, n_ags, n_awk), 'Total infected, students, working')

ut.msg('Number of agents in each category - incorrect:', BLUE)
ut.test_pass(ic.check_agent_numbers(agent_data, n_infected+10, n_ags, n_awk), 'Total infected, students, working')
ut.test_pass(ic.check_agent_numbers(agent_data, n_infected, n_ags-1, n_awk), 'Total infected, students, working')
ut.test_pass(ic.check_agent_numbers(agent_data, n_infected, n_ags, n_awk-1), 'Total infected, students, working')


