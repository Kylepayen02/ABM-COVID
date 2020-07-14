#include "hsp_patient_transitions_tests.h"

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
bool check_isolation(const Agent&, const std::vector<School>,
				const std::vector<Hospital>);
bool check_values(std::vector<double>);
bool check_tests(const Agent& agent, std::vector<double>& time_of_results,
					int& n_hsp, int& n_false_neg, int& n_ih, 
					const std::vector<Hospital> hospitals,
					const std::vector<School> schools);
bool check_fractions(int, int, double, std::string);
bool only_hospitals(const Agent&, const std::vector<Hospital>&, const std::vector<Household>&,
				const std::vector<School>&, const std::vector<Workplace>&);

int main()
{
	test_pass(test_susceptible_transitions(), "Susceptible transitions");
	test_pass(test_exposed_transitions(), "Exposed transitions");
	test_pass(test_symptomatic_transitions(), "Symptomatic transitions");
}

/// Series of tests for transitions of a regular susceptible agent
bool test_susceptible_transitions()
{
	// Time in days
	double dt = 0.25, time = 0.0;
	// Max number of steps to simulate
	int tmax = 200;	

	ABM abm = create_abm(dt);

	// One set of contributions is enough
	abm.compute_place_contributions();

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();
    std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();
    std::vector<Hospital> hospitals = abm.get_copied_vector_of_hospitals();
    Infection infection = abm.get_copied_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	HspPatientTransitions hsp_pt;

	// Verify the outcomes
	int n_infected = 0, n_exposed = 0;
	int n_exposed_never_sy = 0, n_tested_exposed = 0;
	int n_hsp = 0;
	// Various times
	std::vector<double> latency_never_sy = {};
	std::vector<double> latency = {};
	std::vector<double> tinf_never_sy = {};
	std::vector<double> tinf = {};
	std::vector<double> time_to_test = {};
	// Other agent-dependent properties
	std::vector<double> inf_var = {};

	for (int ti = 0; ti<=tmax; ++ti){
		for (auto& agent : agents){
			if (agent.hospital_non_covid_patient() == false)
				continue;
			if (!only_hospitals(agent, hospitals, households, schools, workplaces))
				return false;
			if (agent.infected() == false){
				hsp_pt.susceptible_transitions(agent, time, infection,
					hospitals, infection_parameters, agents);
			}else{
				++n_infected;
				inf_var.push_back(agent.get_inf_variability_factor());
				if (agent.exposed())
					++n_exposed;
				if (agent.tested_exposed()){
					time_to_test.push_back(agent.get_time_of_test());
					++n_tested_exposed;
				}
				if (agent.recovering_exposed() == true){
					latency_never_sy.push_back(agent.get_latency_end_time());
					tinf_never_sy.push_back(agent.get_infectiousness_start_time());
					++n_exposed_never_sy;
				} else if (agent.exposed() && (agent.recovering_exposed() == false)){
					latency.push_back(agent.get_latency_end_time());
					tinf.push_back(agent.get_infectiousness_start_time());
				}
				// No exposed - hospital patient under IH unless shown 
				// COVID-positive
				if (agent.home_isolated())
					return false;
				if (check_isolation(agent, schools, hospitals))
					return false;
				if (agent.tested_in_car())
					return false;

				if (agent.tested_in_hospital()){
					int hID = agent.get_hospital_ID();
					if ( hID > hospitals.size() || hID <= 0)
						return false;
					// The agent should be registered in a 
					// hospital at this point
					if (!find_in_place<Hospital>(hospitals, agent.get_ID(), hID))
						return false;
					++n_hsp;
				}
			}
		}
		time += dt;
	}

	// Infected should be the same as exposed
	if (n_infected != n_exposed)
		return false;
	// Fractions
	if (!check_fractions(n_tested_exposed, n_exposed, 
			infection_parameters.at("exposed fraction to get tested"), "Exposed tested"))
		return false;
	if (n_tested_exposed != n_hsp)
		return false;
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
	if (!check_values(time_to_test))
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
	int tmax = 200;	

	ABM abm = create_abm(dt);

	// One set of contributions is enough
	abm.compute_place_contributions();

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();
    std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();
    std::vector<Hospital> hospitals = abm.get_copied_vector_of_hospitals();
    Infection infection = abm.get_copied_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	HspPatientTransitions hsp_pt;

	// Verify the outcomes
	int n_infected = 0, n_sy = 0, n_exposed = 0;
	int n_tested = 0;
	int n_tested_sy = 0, n_tested_exposed = 0;
	int n_ih = 0, n_hsp = 0, n_false_neg = 0;
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

	for (int ti = 0; ti<=tmax; ++ti){
		for (auto& agent : agents){
			if (agent.hospital_non_covid_patient() == false)
				continue;
			if (agent.infected() == false){
				hsp_pt.susceptible_transitions(agent, time, infection,
					hospitals, infection_parameters, agents);
			}else if (agent.exposed() == true){
				hsp_pt.exposed_transitions(agent, infection, time, dt, 
					households, hospitals, 
					infection_parameters);

				if (agent.infected())
					++n_infected;
				
				if (agent.removed())
					++exp_removed;
		
				if (agent.exposed()){
					++n_exposed;
					latency.push_back(agent.get_latency_end_time());
					tinf.push_back(agent.get_infectiousness_start_time());
					// If tested
					if (agent.tested_exposed() || agent.tested_false_negative()){
						time_to_test.push_back(agent.get_time_of_test());
						++n_tested_exposed;
						++n_tested;
						if (!check_tests(agent, time_to_res, n_hsp, n_false_neg, n_ih, 
											hospitals, schools))
							return false;
					}
					// If recomoved (recovered, never symptomatic)
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
					// If tested
					if (agent.tested()){
						++n_tested_sy;
						++n_tested;
					}
					if (agent.tested() || agent.tested_false_negative()){
						time_to_test.push_back(agent.get_time_of_test());
						if (!check_tests(agent, time_to_res, n_hsp, n_false_neg, n_ih, 
											hospitals, schools))
							return false;
					}
				}
			}
		}
		time += dt;
	}

	// Infected should be sum of exposed and symptomatic 
	if (n_infected != (n_exposed + n_sy))
		return false;
	// Same for tested
	if (n_tested != (n_tested_exposed + n_tested_sy))
		return false;
	// All Sy should be tested
	if (n_tested_sy != n_sy)
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
	int tmax = 500;	

	ABM abm = create_abm(dt);

	// One set of contributions is enough
	abm.compute_place_contributions();

	// Retrieve necessary data
    std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();
    std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();
    std::vector<Hospital> hospitals = abm.get_copied_vector_of_hospitals();
    Infection infection = abm.get_copied_infection_object();
    const std::map<std::string, double> infection_parameters = abm.get_infection_parameters(); 

	// Verify the outcomes
	int n_ih = 0, n_hsp = 0, n_icu = 0; 
	int n_false_neg = 0;
	int n_dying = 0, n_recovering = 0, n_dying_icu = 0;

	// Various times
	std::vector<double> t_icu_to_hsp = {};
	std::vector<double> t_hsp_to_ih = {};
	std::vector<double> t_hsp_to_icu = {};
	std::vector<double> t_ih_to_icu = {};
	std::vector<double> t_ih_to_hsp = {};

	HspPatientTransitions hsp_pt;

	for (int ti = 0; ti<=tmax; ++ti){
		for (auto& agent : agents){
			if (agent.hospital_non_covid_patient() == false)
				continue;
			if (agent.infected() == false){
				hsp_pt.susceptible_transitions(agent, time, infection,
					hospitals, infection_parameters, agents);
			}else if (agent.exposed() == true){
				hsp_pt.exposed_transitions(agent, infection, time, dt, 
					households, hospitals, infection_parameters);
			} else if (agent.symptomatic() == true){
				hsp_pt.symptomatic_transitions(agent, time, dt, infection,  
					households, hospitals, infection_parameters);
		
				// Check for recovery
				if (agent.recovering()){
					++n_recovering;
				}
				if (agent.dying()){
					++n_dying;
					if (agent.hospitalized_ICU())
						++n_dying_icu;
				}
				if (agent.tested_false_negative()){
					++n_false_neg;
					if (check_isolation(agent, schools, hospitals))
						return false;
				}
			}
		}
		time += dt;
	}

	// And all removal 
	if (n_dying == 0 && n_dying_icu == 0 && n_recovering == 0)
		return false;
	
	// Check all the times
	if (!check_values(t_icu_to_hsp))
		return false;
	if (!check_values(t_hsp_to_ih))
		return false;
	if (!check_values(t_hsp_to_icu))
		return false;
	if (!check_values(t_ih_to_icu))
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
	std::string hsp_file("test_data/NR_hospitals.txt");

	// File with infection parameters
	std::string pfname("test_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hsp_file);
	// Then the agents
	abm.create_agents(fin);

	return abm;	
}

/// Check if agent is checked out from all the public places
bool check_isolation(const Agent& agent, const std::vector<School> schools,
				const std::vector<Hospital> hospitals)
{
	const int aID = agent.get_ID();
   	bool not_present_school = true, not_present_work = true;	
	if (agent.student()){
		if (find_in_place<School>(schools, aID, agent.get_school_ID())){
			not_present_school = false;
		}
	}
	if (find_in_place<Hospital>(hospitals, aID, agent.get_hospital_ID()))
			not_present_work = false;

	return (not_present_school && not_present_work);
}

/// Check if values in the vector meet logical criteria
bool check_values(std::vector<double> values)
{
	auto is_negative = [](double val){ return val <= 0; };
	if (std::find_if(values.begin(), values.end(), is_negative) != values.end())
		return false;
	return true;
}

/// Functionality for checking testing-related code
bool check_tests(const Agent& agent, std::vector<double>& time_to_res,
					int& n_hsp, int& n_false_neg, int& n_ih, 
					const std::vector<Hospital> hospitals,
					const std::vector<School> schools)
{
	if (agent.tested_awaiting_results()){
		time_to_res.push_back(agent.get_time_of_results());
	}
	if (agent.tested_in_car())
		return false;
	if (agent.tested() && !agent.tested_awaiting_results() && 
			!agent.tested_in_hospital())
		return false;
	if (agent.tested_in_hospital()){
		int hID = agent.get_hospital_ID();
		if ( hID > hospitals.size() || hID <= 0)
			return false;
		// The agent should be registered in a 
		// hospital at this point
		if (agent.home_isolated()){
			if (find_in_place<Hospital>(hospitals, agent.get_ID(), hID))
				return false;
		}		
		++n_hsp;
	}
	// Check isolation for different scenarions
	if (agent.tested_false_negative()){
		// Should not be in IH anymore
		if (agent.home_isolated() == true)
			return false;
		if (check_isolation(agent, schools, hospitals))
				return false;
		++n_false_neg;
	}else if (!agent.tested_false_negative()){
		// Exposed not confirmed should not be in IH
		if (agent.exposed() && agent.tested()){	
			if (agent.home_isolated() == true)
				return false;
			if (check_isolation(agent, schools, hospitals))
				return false;
		} else if (agent.symptomatic()){
			if (agent.home_isolated() == false)
				return false;
			if (!check_isolation(agent, schools, hospitals))
				return false;
			++n_ih;
		}
	}
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

/// Check if an agent is only registered in a hospital
bool only_hospitals(const Agent& agent, const std::vector<Hospital>& hospitals, 
				const std::vector<Household>& households, const std::vector<School>& schools, 
				const std::vector<Workplace>& workplaces)
{
	if (agent.get_household_ID() > 0)
		if (!find_in_place<Household>(households, agent.get_ID(), agent.get_household_ID()))
			return false;
	if (agent.get_school_ID() > 0 || agent.get_work_ID() > 0)
		return false;
	if (!find_in_place<Hospital>(hospitals, agent.get_ID(), agent.get_hospital_ID()))
		return false;
	return true;
}
