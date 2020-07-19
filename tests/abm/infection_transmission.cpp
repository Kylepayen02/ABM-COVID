#include "abm_tests.h"

/***************************************************** 
 *
 * Test suite for infection related computations
 *
******************************************************/

// Tests
bool abm_contributions_test();

int main()
{
    test_pass(abm_contributions_test(), "Contributions to infection probability");
}

bool abm_contributions_test()
{
	// Create agents
	std::string fin("test_data/infection_tests/agents_test.txt");

	// Files with place info
	std::string hfile("test_data/infection_tests/houses_test.txt");
	std::string sfile("test_data/infection_tests/schools_test.txt");
	std::string wfile("test_data/infection_tests/workplaces_test.txt");

	// Model parameters
	double dt = 0.5;

	// File with infection parameters
	std::string pfname("test_data/infection_tests/wrong_infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/infection_tests/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/infection_tests/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/infection_tests/age_dist_mortality.txt");
	// Map for abm loading of distributions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);


	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);

	// Then the agents
	abm.create_agents(fin);


	// Set infection variability coefficients
	std::vector<double> inf_var = {0.1, 0.2, 0.3};
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
	std::for_each(agents.begin()+6, agents.end(), [](Agent& agent) { agent.set_symptomatic(true); agent.set_exposed(false); });
	// Special cases
	agents.at(15).set_exposed(true);
	agents.at(15).set_symptomatic(false);

	// Contributions
	// Expected
	std::vector<double> ctr_households_exp = {0.056, 0.115};
	std::vector<double> ctr_schools_exp = {0.1376, 0.0, 0.0, 0.0};
	std::vector<double> ctr_workplaces_exp = {0.02, 0.0};
	std::vector<double> ctr_hospitals_exp = {0.182};

	abm.compute_place_contributions();

	// Copies of places objects
    std::vector<Household> households = abm.get_copied_vector_of_households();
    std::vector<School> schools = abm.get_copied_vector_of_schools();    
	std::vector<Workplace> workplaces = abm.get_copied_vector_of_workplaces();

	for (const auto& house : households)
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


	return true;
}



