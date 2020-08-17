#include "contributions_tests.h"

/***************************************************** 
 *
 * Test suite for Contributions class
 *
 ******************************************************/

// Tests
bool contributions_main_test();

// Necessary files
// test_data/houses_test.txt
// test_data/schools_test.txt
// test_data/workplaces_test.txt
// test_data/agents_test.txt
// test_data/sample_infection_parameters.txt
// test_data/age_dist_mortality.txt

int main()
{
	test_pass(contributions_main_test(), "Computations of contributions");
}

// Test for correct computing of infection contributions
bool contributions_main_test()
{
	Contributions contributions;

	// Create agents 
	std::string fin("test_data/agents_test.txt");

	// Files with place info
	std::string hfile("test_data/houses_test.txt");
	std::string sfile("test_data/schools_test.txt");
	std::string wfile("test_data/workplaces_test.txt");

	// Model parameters
	double dt = 0.5;
	double time = 1.0;
	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
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

	// Set infection variability coefficients
	std::vector<double> inf_var = {0.1};
	std::vector<Agent>& agents = abm.get_vector_of_agents_non_const();
	int ind = 0;
	for (auto& agent : agents){
		agent.set_inf_variability_factor(inf_var.at(ind++));
		if (ind > inf_var.size()-1)
			ind = 0;
	}

	// Set agent flags
	// Common
	std::for_each(agents.begin(), agents.begin()+6, [](Agent& agent) { agent.set_exposed(true); });	
	std::for_each(agents.begin()+5, agents.end(), [](Agent& agent) { agent.set_symptomatic(true); agent.set_exposed(false); });
	// Special cases
	agents.at(15).set_exposed(true);
	agents.at(15).set_symptomatic(false);

	// Contributions
	// Expected
	std::vector<double> ctr_households_exp = {0.056, 0.115};
	std::vector<double> ctr_schools_exp = {0.1376, 0.0, 0.0, 0.0};
	std::vector<double> ctr_workplaces_exp = {0.02, 0.0};

	// Copies of places objects
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();    
	std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();

	for (const auto& agent : agents){

		// Removed and susceptible don't contribute
		if (agent.removed() == true){
			continue;
		}
		if (agent.infected() == false){
			continue;
		}
			
		// Consider all infectious cases, raise 
		// exception if no existing case
		if (agent.exposed() == true){
			contributions.compute_exposed_contributions(agent, time, households, 
							schools, workplaces);
		}else if (agent.symptomatic() == true){
			contributions.compute_symptomatic_contributions(agent, time, households, 
							schools, workplaces);
		}else{
			throw std::runtime_error("Agent does not have any state");
		}
	}
	contributions.total_place_contributions(households, schools, 
											workplaces);

/*	for (const auto& house : households)
		if (!float_equality<double>(house.get_infected_contribution(), 
								ctr_households_exp.at(house.get_ID()-1), 1e-3))
			return false;
	for (const auto& school : schools)
		if (!float_equality<double>(school.get_infected_contribution(),
								ctr_schools_exp.at(school.get_ID()-1), 1e-3))
			return false;
	for (const auto& workplace : workplaces)
		if (!float_equality<double>(workplace.get_infected_contribution(),
								ctr_workplaces_exp.at(workplace.get_ID()-1), 1e-3))
			return false;
*/	
	// Resetting test
	contributions.reset_sums(households, schools, workplaces);
	for (const auto& house : households)
		if (!float_equality<double>(house.get_infected_contribution(), 0.0, 1e-5)){
			std::cout << house.get_infected_contribution() << std::endl;
			return false;
		}
	for (const auto& school : schools)
		if (!float_equality<double>(school.get_infected_contribution(), 0.0, 1e-5))
			return false;
	for (const auto& workplace : workplaces)
		if (!float_equality<double>(workplace.get_infected_contribution(), 0.0, 1e-5))
			return false;

	return true;
}
