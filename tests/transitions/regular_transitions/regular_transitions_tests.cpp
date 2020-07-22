#include "regular_transitions_tests.h"

/***************************************************** 
 *
 * Test suite for RegularTransitions class
 *
 ******************************************************/

// Tests
bool test_susceptible_transitions();
bool test_exposed_transitions();
bool test_symptomatic_transitions();

// Supporting functions
ABM create_abm(const double dt);
bool check_values(std::vector<double>);
bool check_fractions(int, int, double, std::string);

int main()
{
	test_pass(test_susceptible_transitions(), "Susceptible transitions");
	test_pass(test_exposed_transitions(), "Exposed transitions");
	test_pass(test_symptomatic_transitions(), "Symptomatic transitions");
}

/// Series of tests for transitions of a regular susceptible agent
bool test_susceptible_transitions()
{
	// Time in days, space in km
	double dt = 0.25, time = 0.0;
	// Max number of steps to simulate
	int tmax = 100;	

	ABM abm = create_abm(dt);

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();
    std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();
	Infection infection = abm.get_copied_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	RegularTransitions regular;

	for (int ti = 0; ti<=tmax; ++ti){
		
		abm.compute_place_contributions();
 		
		households = abm.get_copied_vector_of_households();
    	schools = abm.get_copied_vector_of_schools();
    	workplaces = abm.get_copied_vector_of_workplaces();

		for (auto& agent : agents){
			if (agent.infected() == false){
				regular.susceptible_transitions(agent, time, infection,
					households, schools, workplaces,
					infection_parameters, agents);
			}
		}
		time += dt;
	}

	// Verify the outcomes
	int n_infected = 0, n_exposed = 0;
	int n_exposed_never_sy = 0;
	// Various times
	std::vector<double> latency_never_sy = {};
	std::vector<double> latency = {};
	std::vector<double> tinf_never_sy = {};
	std::vector<double> tinf = {};
	// Other agent-dependent properties
	std::vector<double> inf_var = {};

	for (const auto& agent : agents){
		if (agent.infected()){
			inf_var.push_back(agent.get_inf_variability_factor());
			++n_infected;
		}
		if (agent.exposed())
			++n_exposed;
		if (agent.recovering_exposed() == true){
			latency_never_sy.push_back(agent.get_latency_end_time());
			tinf_never_sy.push_back(agent.get_infectiousness_start_time());
			++n_exposed_never_sy;
		} else if (agent.exposed() && (agent.recovering_exposed() == false)){
			latency.push_back(agent.get_latency_end_time());
			tinf.push_back(agent.get_infectiousness_start_time());
		}
	}		

	// Infected should be the same as exposed
	if (n_infected != n_exposed)
		return false;
	// Fractions
	if (!check_fractions(n_exposed_never_sy, n_exposed, 
			infection_parameters.at("fraction exposed never symptomatic"), "fraction exposed never symptomatic"))
		return false;

	// Check all the times
	if (!check_values(latency_never_sy))
		return false;
	if (!check_values(latency))
		return false;
	if (!check_values(tinf))
		return false;
	if (!check_values(tinf_never_sy))
		return false;
	// And variability
	if (!check_values(inf_var))
		return false;

	return true;
}

/// Series of tests for transitions of a regular exposed agent
bool test_exposed_transitions()
{
	// Time in days, space in km
	double dt = 0.25, time = 0.0;
	// Max number of steps to simulate
	int tmax = 500;	

	ABM abm = create_abm(dt);

	abm.compute_place_contributions();

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();
    std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();
    Infection infection = abm.get_copied_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	RegularTransitions regular;

	for (int ti = 0; ti<=tmax; ++ti){
		for (auto& agent : agents){
			if (agent.infected() == false){
				regular.susceptible_transitions(agent, time, infection,
					households, schools, workplaces,
					infection_parameters, agents);
			}else if (agent.exposed() == true){
				regular.exposed_transitions(agent, infection, time, dt, 
					households, schools, workplaces,
					infection_parameters);
			}
		}
		time += dt;
	}

	// Verify the outcomes
	int n_infected = 0, n_sy = 0, n_exposed = 0;
	int n_removed = 0, exp_removed = 0;
	int n_dying = 0, n_recovering = 0;
	// Various times
	std::vector<double> latency = {};
	std::vector<double> tinf = {};
	std::vector<double> time_to_test = {};
	std::vector<double> time_to_res = {};
	std::vector<double> time_to_death = {};
	std::vector<double> time_to_recovery = {};
	double exp_recovery = infection_parameters.at("recovery time");

	for (const auto& agent : agents){

		if (agent.infected()){
			++n_infected;
		}
	
		if (agent.removed())
			++exp_removed;

		if (agent.exposed()){
			++n_exposed;
			latency.push_back(agent.get_latency_end_time());
			tinf.push_back(agent.get_infectiousness_start_time());
			// If removed (recovered, never symptomatic)
			if ((agent.get_latency_end_time() <= time) &&
					(agent.recovering_exposed()))
				++n_removed;
		}else if (agent.symptomatic()){
			++n_sy;
			// Check for recovery
			if (agent.recovering()){
				++n_recovering;
				time_to_recovery.push_back(agent.get_recovery_time());
			}
			if (agent.dying()){
				++n_dying;
				time_to_death.push_back(agent.get_time_of_death());
			}
		}
	}		
	// Infected should be sum of exposed and symptomatic 
	if (n_infected != (n_exposed + n_sy))
		return false;

	// Removal
	if (n_sy != (n_dying + n_recovering))
			return false;

	// Check all the times
	if (!check_values(latency))
		return false;
	if (!check_values(tinf))
		return false;
	if (!check_values(time_to_test))
		return false;
	if (!check_values(time_to_res))
		return false;
	if (!check_values(time_to_death))
		return false;
			
	return true;
}

/// Series of tests for transitions of a regular symptomatic agent
bool test_symptomatic_transitions()
{
	// Time in days, space in km
	double dt = 0.25, time = 0.0;
	// Max number of steps to simulate
	int tmax = 100;	

	ABM abm = create_abm(dt);

	// One set of contributions is enough
	abm.compute_place_contributions();

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();
    std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();
    Infection infection = abm.get_copied_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	// Verify the outcomes
	int n_dying = 0, n_recovering = 0;


	RegularTransitions regular;

	for (int ti = 0; ti<=tmax; ++ti){
		for (auto& agent : agents){
			if (agent.infected() == false){
				regular.susceptible_transitions(agent, time, infection,
					households, schools, workplaces,
					infection_parameters, agents);
			}else if (agent.exposed() == true){
				regular.exposed_transitions(agent, infection, time, dt, 
					households, schools, workplaces,
					infection_parameters);
			} else if (agent.symptomatic() == true){
				regular.symptomatic_transitions(agent, time, dt, infection,  
					households, schools, workplaces,
					infection_parameters);
		
				// Check for recovery
				if (agent.recovering()){
					++n_recovering;
				}
				if (agent.dying()){
					++n_dying;
				}
			}
		}
		time += dt;
	}
	// And all removal
	if (n_dying == 0 || n_recovering == 0)
		return false;
		
	return true;
}
// Common operations for creating the ABM interface
ABM create_abm(const double dt)
{
	// Input files
	std::string fin("test_data/NR_agents.txt");
	std::string hfile("test_data/NR_households.txt");
	std::string sfile("test_data/NR_schools.txt");
	std::string wfile("test_data/NR_workplaces.txt");

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"mortality", dmort_name} };

	ABM abm(dt, pfname, dfiles);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	// Then the agents
	abm.create_agents(fin);

	return abm;	
}


/// Check if values in the vector meet logical criteria
bool check_values(std::vector<double> values)
{
	auto is_negative = [](double val){ return val <= 0; };
	if (std::find_if(values.begin(), values.end(), is_negative) != values.end())
		return false;
	return true;
}


/// Check if num1/num2 is roughly equal to expected
bool check_fractions(int num1, int num2, double fr_expected, std::string msg)
{
	double fr_tested = static_cast<double>(num1)/static_cast<double>(num2);
	if (!float_equality<double>(fr_tested, fr_expected, 0.1)){
		std::cout << msg << std::endl;
		std::cout << "Computed: " << fr_tested << " Expected: " << fr_expected << std::endl; 
		return false;
	}
	return true;
}


