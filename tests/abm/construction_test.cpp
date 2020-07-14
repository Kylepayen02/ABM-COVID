#include "abm_tests.h"

/***************************************************** 
 *
 * Test suite for creation of ABM objects
 *
 ******************************************************/

// Tests
bool create_households_test();
bool create_schools_test();
bool wrong_school_type_test();
bool create_workplaces_test();
bool create_hospitals_test();
bool create_agents_test();
bool create_infection_model_test(const std::string = {}, const std::string = {});

// Supporting functions
bool compare_places_files(std::string fname_in, std::string fname_out, 
				const std::vector<double> infection_parameters, const bool has_type = false);
bool compare_agents_files(std::string fname_in, std::string fname_out);
bool correctly_registered(const ABM, const std::vector<std::vector<int>>, 
							const std::vector<std::vector<int>>, std::string, 
							const std::string, const std::string, const int);

// Necessary files
// test_data/houses_test.txt
// test_data/schools_test.txt
// test_data/workplaces_test.txt
// test_data/hospitals_test.txt
// test_data/agents_test.txt
// test_data/sample_infection_parameters.txt
// test_data/age_dist_hospitalization.txt
// test_data/age_dist_hosp_ICU.txt
// test_data/age_dist_mortality.txt

// Files to delete before running 
// test_data/houses_out.txt
// test_data/schools_out.txt
// test_data/workplaces_out.txt
// test_data/hospitals_out.txt
// test_data/agents_out.txt

int main()
{
	test_pass(create_households_test(), "Household creation");
	test_pass(create_schools_test(), "School creation");
	test_pass(wrong_school_type_test(), "Wrong school type detection");
	test_pass(create_workplaces_test(), "Workplace creation");
	test_pass(create_hospitals_test(), "Hospitals creation");
	test_pass(create_agents_test(), "Agent creation");

	std::string outfile_1 = "test_data/output_infection_parameters.txt";	
	std::string outfile_2 = "test_data/output_age_distributions.txt";
	test_pass(create_infection_model_test(outfile_1, outfile_2), "Infection model creation");
}

// Checks household creation from file
bool create_households_test()
{
	// Create households and save the information
	std::string fin("test_data/houses_test.txt");
	std::string fout("test_data/houses_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);

	abm.create_households(fin);
	abm.print_households(fout);
	
	// Vector of infection parameters 
	// (order as in output file: ck, beta, alpha)
	std::vector<double> infection_parameters = {2.0, 0.49, 0.80, 0.4}; 

	// Check if correct, hardcoded for places properties
	return compare_places_files(fin, fout, infection_parameters);
}


// Checks school creation from file
bool create_schools_test()
{
	// Create schools and save the information
	std::string fin("test_data/schools_test.txt");
	std::string fout("test_data/schools_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);
	abm.create_schools(fin);
	abm.print_schools(fout);

	bool has_type = true;

	// Vector of infection parameters 
	// (order as in output file: ck, beta, psi for middle)
	std::vector<double> infection_parameters = {2.0, 0.94, 0.2}; 

	// Check if correct, hardcoded for places properties
	return compare_places_files(fin, fout, infection_parameters, has_type);
}

// Tests if exception is triggered upon entering a wrong school type
bool wrong_school_type_test()
{
	bool verbose = true;
	const std::invalid_argument invarg("Wrong type");

	std::string fin("test_data/schools_wrong_type.txt");
	double dt = 2.0;
	std::string pfname("test_data/sample_infection_parameters.txt");
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);
	
	if (!exception_test(verbose, &invarg, &ABM::create_schools, abm, fin))
		return false;
	
	return true;
}

// Checks workplace creation from file
bool create_workplaces_test()
{
	// Create workplaces and save the information
	std::string fin("test_data/workplaces_test.txt");
	std::string fout("test_data/workplaces_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);

	abm.create_workplaces(fin);
	abm.print_workplaces(fout);
	
	// Vector of infection parameters 
	// (order as in output file: ck, beta, psi)
	std::vector<double> infection_parameters = {2.0, 0.47, 0.5}; 

	// Check if correct, hardcoded for places properties
	return compare_places_files(fin, fout, infection_parameters);	
}

// Checks hospital creation from file
bool create_hospitals_test()
{
	// Create workplaces and save the information
	std::string fin("test_data/hospitals_test.txt");
	std::string fout("test_data/hospitals_out.txt");

	// Model parameters
	// Time step
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);

	abm.create_hospitals(fin);
	abm.print_hospitals(fout);
	
	// Vector of infection parameters 
	// (order as in output file: ck, betas for each category)
	std::vector<double> infection_parameters = {2.0, 0.47, 0.2009, 0.1, 0.90, 0.70}; 

	// Check if correct, hardcoded for places properties
	return compare_places_files(fin, fout, infection_parameters);	
}

// Checks agents creation from file including proper 
// distribution into places 
bool create_agents_test()
{
	// Create agents and save the information
	std::string fin("test_data/agents_test.txt");
	std::string fout("test_data/agents_out.txt");

	// Files with place info
	std::string hfile("test_data/houses_test.txt");
	std::string sfile("test_data/schools_test.txt");
	std::string wfile("test_data/workplaces_test.txt");
	std::string hspfile("test_data/hospitals_test.txt");

	// Files for checking registration failures
	std::string hinfo("test_data/houses_basic_info.txt");
	std::string sinfo("test_data/schools_basic_info.txt");
	std::string winfo("test_data/works_basic_info.txt");
	std::string hsp_info("test_data/hospitals_basic_info.txt");

	// .. including correct registered agents IDs
	std::string hagnt("test_data/houses_agent_IDs.txt");
	std::string sagnt("test_data/schools_agent_IDs.txt");
	std::string wagnt("test_data/works_agent_IDs.txt");
	std::string hsp_agnt("test_data/hospitals_agent_IDs.txt");

	// Model parameters
	double dt = 2.0;
	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
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
	abm.create_hospitals(hspfile);

	// Then the agents
	abm.create_agents(fin);
	abm.print_agents(fout);
	
	// Check if correct, hardcoded for agents properties
	if (!compare_agents_files(fin, fout))
		return false;

	// Check if correctly registered in every type of place
	// Vectors with expected number of agents nd infected
	// agents in each place (first entry total, second - total infected)
	std::vector<std::vector<int>> houses = {{3, 1}, {1, 0}, {1, 0}};
	std::vector<std::vector<int>> schools = {{2, 1}, {0, 0}, {1, 0}, {0, 0}};
	std::vector<std::vector<int>> workplaces = {{0, 0}, {1, 0}};
	std::vector<std::vector<int>> hospitals = {{1, 0}, {1, 0}};

	// Expected agents in each place by ID
	std::vector<std::vector<int>> houses_agents = {{4, 5, 6}, {2}, {3}};
	std::vector<std::vector<int>> schools_agents = {{4, 5}, {0}, {3}, {0}};
	std::vector<std::vector<int>> workplaces_agents = {{0}, {2}};
	std::vector<std::vector<int>> hospital_agents = {{3}, {1}};

	if (!correctly_registered(abm, houses, houses_agents, "House", hinfo, hagnt, 4))
		return false;
	if (!correctly_registered(abm, schools, schools_agents, "school", sinfo, sagnt, 3))
		return false;
	if (!correctly_registered(abm, workplaces, workplaces_agents, "Workplace", winfo, wagnt, 3))
		return false;
	if (!correctly_registered(abm, hospitals, hospital_agents, "hospital", hsp_info, hsp_agnt, 6))
		return false;
	 
	return true;
}

/// \brief Demonstrates loading of COVID parameters and distributions
/// \details This doesn't really test, testing is done in specific 
///		objects that use the loaded paramters 
bool create_infection_model_test(const std::string outfile_1, const std::string outfile_2)
{
	// Model parameters
	// Time step
	double dt = 2.0;

	// File with infection parameters
	std::string pfname("test_data/sample_infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("test_data/age_dist_hospitalization.txt");
	std::string dhicu_name("test_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("test_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);

 	// Optionally print the paramters
	if (!outfile_1.empty())
		abm.print_infection_parameters(outfile_1);	
	
	// Optionally print the age-dependent distributions	
	if (!outfile_2.empty())
		abm.print_age_dependent_distributions(outfile_2);

	return true;
}

/// \brief Compare input places file with output from a Place object 
bool compare_places_files(std::string fname_in, std::string fname_out, 
				const std::vector<double> infection_parameters, const bool has_type)
{
	std::ifstream input(fname_in);
	std::ifstream output(fname_out);

	// Load the first file as vectors
	std::vector<int> in_IDs;
	std::vector<double> in_coords_x, in_coords_y;

	int ID = 0;
	double x = 0.0, y = 0.0;
	std::string type = {};
	if (has_type == false){
		while (input >> ID >> x >> y){
			in_IDs.push_back(ID);
			in_coords_x.push_back(x);
			in_coords_y.push_back(y);
		}
	}else{
		// For places that have types
		while (input >> ID >> x >> y >> type){
			in_IDs.push_back(ID);
			in_coords_x.push_back(x);
			in_coords_y.push_back(y);
		}
	}

	// Now load the second (output) and compare
	// Also check if total number of agents and infected agents is 0
	int num_ag = 0, num_inf = 0;
	int num_ag_exp = 0, num_inf_exp = 0;
	int ind = 0;
	double parameter;

	while (output >> ID >> x >> y >> num_ag >> num_inf){
		// Compare ID, location, agents
		if (ID != in_IDs.at(ind))
			return false;
		if (!float_equality<double>(x, in_coords_x.at(ind), 1e-5))
			return false;
		if (!float_equality<double>(y, in_coords_y.at(ind), 1e-5))
			return false;
		if (num_ag != num_ag_exp)
			return false;
		if (num_inf != num_inf_exp)
			return false;
		// Compare infection parameters
		for (auto const& expected_parameter : infection_parameters){
			output >> parameter;
			if (!float_equality<double>(expected_parameter, parameter, 1e-5))
				return false;
		}
		++ind;
	}

	// In case file empty, shorter, or doesn't exist
	if (in_IDs.size() != ind)
		return false;

	return true;	
}

/// \brief Compare input agent file with output from an Agent object 
bool compare_agents_files(std::string fname_in, std::string fname_out)
{
	std::ifstream input(fname_in);
	std::ifstream output(fname_out);

	// Load the first file as vectors and generate IDs
	std::vector<int> IDs;
	// student, works, hospital staff, hospital patient
	std::vector<std::vector<bool>> status;
	std::vector<bool> infected;
	std::vector<int> age;
	std::vector<std::vector<double>> position;
	// house ID, school ID, work ID, hospital ID
	std::vector<std::vector<int>> places;		 
	
	int ID = 0, yrs = 0, hID = 0, sID = 0, wID = 0, hspID = 0;
	bool studies = false, works = false, is_infected = false;
	bool works_at_hospital = false, non_covid_patient = false;
	double x = 0.0, y = 0.0;

	while (input >> studies >> works >> yrs 
			>> x >> y >> hID >> non_covid_patient >> sID >> wID 
			>> works_at_hospital >> hspID >> is_infected)
	{
		IDs.push_back(++ID);
		status.push_back({studies, works, works_at_hospital, non_covid_patient});
		infected.push_back(is_infected);
		age.push_back(yrs);
		position.push_back({x, y});
		places.push_back({hID, sID, wID, hspID});
	}

	// Now load the second (output) file and compare
	int ind = 0;
	while (output >> ID >> studies >> works >> yrs 
			>> x >> y >> hID >> non_covid_patient >> sID >> wID 
			>> works_at_hospital >> hspID >> is_infected)
	{
		if (ID != IDs.at(ind))
			return false;
		if (studies != status.at(ind).at(0) 
				|| works != status.at(ind).at(1)
				|| works_at_hospital != status.at(ind).at(2)
				|| non_covid_patient != status.at(ind).at(3))
			return false;
		if (is_infected != infected.at(ind))
			return false;
		if (yrs != age.at(ind))
			return false;
		if (!float_equality<double>(x, position.at(ind).at(0), 1e-5))
			return false;
		if (!float_equality<double>(y, position.at(ind).at(1), 1e-5))
			return false;
		if (hID != places.at(ind).at(0) 
				|| sID != places.at(ind).at(1) 
				|| wID != places.at(ind).at(2)
				|| hspID != places.at(ind).at(3))
			return false;
		++ind;
	}

	// In case file empty, shorter, or doesn't exist
	//if (IDs.size() != ind)
	//	return false;
	return true;	
}

/**
 * \brief Check if agents registered correctly in a given Place
 *
 * @param abm - an ABM object
 * @param place_info - vector of expected total number of agents in each place and total infected
 * @param place_agents - vector with IDs of agents in each place
 * @param place_type - type of place (house, school or work), case independent
 * @param info_file - name of the file to save basic info output to 
 * @param agent_file - name of the file to save agent IDs to
 * @param num_red_args - number of redundant arguments (i.e. infection paramters) in printed
 */ 
bool correctly_registered(const ABM abm, const std::vector<std::vector<int>> place_info, 
							const std::vector<std::vector<int>> place_agents, std::string place_type, 
							const std::string info_file, const std::string agent_file, const int num_red_args)
{

	// Save basic info and agent IDs
	place_type = str_to_lower(place_type);
	if (place_type == "house"){
		abm.print_households(info_file);
		abm.print_agents_in_households(agent_file);
	} else if (place_type == "school"){
		abm.print_schools(info_file);
		abm.print_agents_in_schools(agent_file);
	} else if (place_type == "workplace"){
		abm.print_workplaces(info_file);
		abm.print_agents_in_workplaces(agent_file);
	} else if (place_type == "hospital"){
		abm.print_hospitals(info_file);
		abm.print_agents_in_hospitals(agent_file);
	} else{
		std::cout << "Wrong place type" << std::endl;
		return false; 
	}
	
	// Check if total number of agents and infected agents are correct
	std::ifstream info_total(info_file);
	int ID = 0, num_agents = 0, num_inf = 0;
	double x = 0.0, y = 0.0, not_needed_arg = 0.0;
	int ind = 0;
	while (info_total >> ID >> x >> y >> num_agents >> num_inf){
		// Ignore remaining 
		for (int i=0; i<num_red_args; ++i)
			info_total >> not_needed_arg;

		if (num_agents != place_info.at(ind).at(0))
			return false;
		if (num_inf != place_info.at(ind).at(1))
			return false;		
		++ind;
	}

	// Check if correct agent IDs
	// Load ID file into a nested vector, one inner vector per place
	std::vector<std::vector<int>> saved_IDs;
	std::ifstream info_IDs(agent_file);
	std::string line;
	while (std::getline(info_IDs, line))
	{	
		std::istringstream iss(line);
		std::vector<int> place_IDs;
		while (iss >> ID)
			place_IDs.push_back(ID);
		saved_IDs.push_back(place_IDs);
	}

	// Compare the vectors
	return is_equal_exact<int>(place_agents, saved_IDs);
}

