# ------------------------------------------------------------------
#
#	Create a random (i.e. not real) population for the first runs
#
# ------------------------------------------------------------------

import sys
py_path = '../../../scripts/'
sys.path.insert(0, py_path)

import create_population as cpop

#
# Population settings 
#

# Number of agents and initially infected agents
n_agents = 10000
# 10, 100, 1000
n_infected = 100	

# Number of agents in schools and workplaces
n_ags = 3000
n_awk = 7000

# Number of houses, schools, and workplaces
# 250, 2500, 25000
nh = 2500
# 1, 5, 50
# New: (with beta_h = 0.09)
# 1, 50, 500
ns = 5
# 5, 50, 500
# New: (with beta_h = 0.09)
# Same parameters as before
nw = 50

# Spatial limits, km
# Poulation density swap:
# 319.35, 3193.5, 31935.0 
sp_lim_x = [0, 1.4804]
sp_lim_y = [100, 102.11491]

#
# Generate
#

houses = cpop.Houses(nh, sp_lim_x, sp_lim_y)
schools = cpop.Schools(ns, sp_lim_x, sp_lim_y)	
works = cpop.Works(nw, sp_lim_x, sp_lim_y)
	
agents = cpop.Agents(n_agents, nh, n_ags, n_awk, n_infected)
agents.distribute(houses, schools, works)

#
# Save to files
#
		
with open('input_data/agents_data.txt', 'w') as fout:
	fout.write(repr(agents))

with open('input_data/houses_data.txt', 'w') as fout:
	fout.write(repr(houses))

with open('input_data/schools_data.txt', 'w') as fout:
	fout.write(repr(schools))

with open('input_data/workplaces_data.txt', 'w') as fout:
	fout.write(repr(works))

