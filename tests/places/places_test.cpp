#include "places_tests.h"

/***************************************************** 
 *
 * Test suite for functionality of the Place class
 * 	 and its derived classes
 *
******************************************************/

// Tests
bool place_test();
bool school_test();
bool workplace_test();
bool household_test();
bool hospital_test();

// Tests for contributions
bool contribution_test_hospitals();
bool contribution_test_general_place();
bool contribution_test_workplace();
bool contribution_test_school();
bool contribution_test_household();

// Supporting functions
bool general_place_test(Place&, const int, const double, const double,
							const double, const double, const double = -0.1,
							const double = -0.1, const double = -0.1,
							std::map<const std::string, const double> = {});
bool general_contribution_test(Place&, double, int, int, int, double);
bool household_contribution_test(Household&, double, int, int, int, double);

int main()
{
	test_pass(place_test(), "Place class functionality");
	test_pass(contribution_test_general_place(), "Contribution test for places");

	test_pass(school_test(), "School class functionality");
	test_pass(contribution_test_school(), "Contribution test for schools");

	test_pass(workplace_test(), "Workplace class functionality");
	test_pass(contribution_test_workplace(), "Contribution test for workplace");

	test_pass(hospital_test(), "Hospital class functionality");
	test_pass(contribution_test_hospitals(), "Contribution test for hospitals");
	
	test_pass(household_test(), "Household class functionality");
	test_pass(contribution_test_household(), "Contribution test for household");
}

/// Tests all public functions from the Place class  
bool place_test()
{
	int pID = 1030; 
	double xi = 0.5, yi = 100.1;
	double severity_cor = 2.0, beta = 0.47;

	Place place(pID, xi, yi, severity_cor, beta);

	if (!general_place_test(place, pID, xi, yi, severity_cor, beta))
		return false;

	return true;
}

/// Test contribution computation
bool contribution_test_general_place()
{
	int pID = 1030; 
	double xi = 0.5, yi = 100.1;
	double severity_cor = 2.0, beta = 0.47, inf_var = 0.9;

	int n_exp = 3, n_sym = 6, n_tot = 15;
	double exp_lambda = 0.4230;

	Place place(pID, xi, yi, severity_cor, beta);

	if (!general_contribution_test(place, inf_var, n_exp, n_sym, n_tot, exp_lambda))
		return false;

	return true;
}

/// Tests all public functions from the School class  
bool school_test()
{
	int pID = 130; 
	double xi = 0.05, yi = 0.134;
	double severity_cor = 2.0, beta = 0.47;
	double psi_s = 0.25;

	School school(pID, xi, yi, severity_cor, psi_s, beta);

	if (!general_place_test(school, pID, xi, yi, severity_cor, beta, -1.0, -1.0, psi_s))
		return false;

	return true;
}

/// Test contribution computation
bool contribution_test_school()
{
	int pID = 130, type = 2; 
	double xi = 0.05, yi = 0.134;
	double severity_cor = 2.0, beta = 0.4;
	double psi_s = 0.25, inf_var = 0.9;

	int n_exp = 3, n_sym = 6, n_tot = 15;
	double exp_lambda = 0.1440;

	School school(pID, xi, yi, severity_cor, psi_s, beta);

	if (!general_contribution_test(school, inf_var, n_exp, n_sym, n_tot, exp_lambda))
		return false;

	return true;
}

/// Tests all public functions from the Workplace class  
bool workplace_test()
{
	int pID = 1076; 
	double xi = 50.901, yi = 1001.675;
	double severity_cor = 2.5, beta = 0.49;
	double psi_w = 0.2;

	Workplace work(pID, xi, yi, severity_cor, psi_w, beta);

	if (!general_place_test(work, pID, xi, yi, severity_cor, beta, -1.0, -1.0, psi_w))
		return false;

	return true;
}

/// Test contribution computation
bool contribution_test_workplace()
{
	int pID = 130, type = 2; 
	double xi = 0.05, yi = 0.134;
	double severity_cor = 2.5, beta = 0.49;
	double psi_w = 0.2, inf_var = 1.3;

	int n_exp = 3, n_sym = 6, n_tot = 10;
	double exp_lambda = 0.3822;

	Workplace work(pID, xi, yi, severity_cor, psi_w, beta);

	if (!general_contribution_test(work, inf_var, n_exp, n_sym, n_tot, exp_lambda))
		return false;

	return true;
}

/// Tests all public functions from the Hospital class  
bool hospital_test()
{
	int pID = 10760; 
	double xi = 5.901, yi = 10010.675;
	double severity_cor = 2.5;
	std::map<const std::string, const double> betas = {{"hospital employee", 0.5}, 
							{"hospital non-COVID patient", 1.3}, 
							{"hospital testee", 0.009}, {"hospitalized", 0.1},
							{"hospitalized ICU", 0.07}};
	

	Hospital hospital(pID, xi, yi, severity_cor, betas);
	
	if (!general_place_test(hospital, pID, xi, yi, severity_cor, 0.0, -1.0, -1.0, -1.0,  betas))
		return false;

	return true;
}

/// Test computation of probability contribution in hospitals
bool contribution_test_hospitals()
{
	int pID = 10760; 
	double xi = 5.901, yi = 10010.675;
	int ntot = 20.0;
	double severity_cor = 2.5;
	double inf_var = 0.2;
	int n_exposed = 5, n_hospitalized = 3;
	std::map<const std::string, const double> betas = {{"hospital employee", 0.5}, 
							{"hospital non-COVID patient", 1.3}, 
							{"hospital testee", 0.009}, {"hospitalized", 0.1},
							{"hospitalized ICU", 0.07}};
	
	// Expected fraction added to total lambda and computed lambda initialization
	double exp_lambda = 0.0475, lambda = 0.0;

	// Check contributions
	// Set total number of agents
	Hospital hospital(pID, xi, yi, severity_cor, betas);

	for (int i=0; i<ntot; ++i)
		hospital.add_agent(i+1);

	for (int i=0; i<(n_exposed+n_hospitalized); ++i){
		if (i < n_exposed)
			hospital.add_exposed(inf_var);
		else if (i >= n_exposed)
			hospital.add_hospitalized(inf_var);	
	}

	// The rest has only one agent
	hospital.add_exposed_patient(inf_var);
	hospital.add_hospital_tested(inf_var);
	hospital.add_hospitalized_ICU(inf_var);

	hospital.compute_infected_contribution();

	lambda = hospital.get_infected_contribution();
	if (!float_equality<double>(exp_lambda, lambda, 1e-3))
		return false;

	return true;
}

/// Tests all public functions from the Household class  
bool household_test()
{
	int pID = 176; 
	double xi = 5.95671, yi = 11.00675;
	double severity_cor = 3.5, beta = 0.90, beta_ih = 0.4;
	double alpha = 0.7;

	Household house(pID, xi, yi, alpha, severity_cor, beta, beta_ih);

	// All other tests including the default contribution
	if (!general_place_test(house, pID, xi, yi, severity_cor, beta, alpha, beta_ih))
		return false;
	return true;
}

/// Test contribution computation
bool contribution_test_household()
{
	int pID = 176; 
	double xi = 5.95671, yi = 11.00675;
	double severity_cor = 3.5, beta = 0.90, beta_ih = 0.4;
	double alpha = 0.7;
	double inf_var = 0.5;

	int n_exp = 2, n_sym = 1, n_tot = 5;
	double exp_lambda = 1.0291;

	Household house(pID, xi, yi, alpha, severity_cor, beta, beta_ih);

	if (!household_contribution_test(house, inf_var, n_exp, n_sym, n_tot, exp_lambda))
		return false;

	return true;
}

/** 
 * \brief Test for general contribution to infection probability
 * 
 * @param place - place object
 * @param inf_var - infection variability
 * @param n_exp - number of exposed
 * @param n_sym - number of symptomatic
 * @param n_tot - total number of agents
 * @param exp_lambda - expected lambda
 */
bool general_contribution_test(Place& place, double inf_var, 
				int n_exp, int n_sym, int n_tot, double exp_lambda)
{
	double lambda = 0.0;

	for (int i=0; i<n_tot; ++i)
		place.add_agent(i+1);

	for (int i=0; i<(n_exp + n_sym); ++i){
		if (i < n_exp)
			place.add_exposed(inf_var);
		else if (i >= n_exp)
			place.add_symptomatic(inf_var);	
	}

	place.compute_infected_contribution();

	lambda = place.get_infected_contribution();
	if (!float_equality<double>(exp_lambda, lambda, 1e-3))
		return false;

	return true;
}

/// Test for most classes derived from Place including itself
bool general_place_test(Place& place, const int pID, 
		const double xi, const double yi, const double ck, const double beta,
		const double alpha, const double beta_ih, const double psi, std::map<const std::string, const double> hospital_betas)
{
	//
	// Test if correct without agents
	// Uses streams to avoid going through files
	//
	
	int ntot = 0, ninf = 0;
	int test_pID = 0, test_ntot = 0, test_ninf = 0; 
	double test_x = 0.0, test_y = 0.0;
	double test_ck = 0.0, test_beta = 0.0;
	double test_alpha = 0.0, test_beta_ih = 0.0, test_psi = 0.0;
	double test_beta_employee = 0.0, test_beta_patient = 0.0;
	double test_beta_testee = 0.0, test_beta_hospitalized = 0.0;
	double test_beta_ICU = 0.0;

	std::stringstream place_buff;
	place_buff << place;
	std::istringstream res(place_buff.str());

	// If not a house, school, workplace, or hospital
	if (alpha < 0.0 && psi < 0.0 && hospital_betas.size() == 0)
		res >> test_pID >> test_x >> test_y >> test_ntot >> test_ninf >> test_ck >> test_beta;
	// If a house
	if (alpha > 0.0)
		res >> test_pID >> test_x >> test_y >> test_ntot >> test_ninf >> test_ck >> test_beta >> test_alpha >> test_beta_ih;
	// If a school or workplace
	if (psi > 0.0)
		res >> test_pID >> test_x >> test_y >> test_ntot >> test_ninf >> test_ck >> test_beta >> test_psi;
	// If a hospital
	if (hospital_betas.size() > 0)
		res >> test_pID >> test_x >> test_y >> test_ntot >> test_ninf >> test_ck 
			>> test_beta_employee >> test_beta_patient >> test_beta_testee
			>> test_beta_hospitalized >> test_beta_ICU;

	// Check each value
	if (pID != test_pID || ntot != test_ntot || ninf != test_ninf)
		return false;
	if (!float_equality<double>(xi, test_x, 1e-5))
		return false;
	if (!float_equality<double>(yi, test_y, 1e-5))
		return false;
	if (!float_equality<double>(ck, test_ck, 1e-5))
		return false;
	if (hospital_betas.size() == 0 && !float_equality<double>(beta, test_beta, 1e-5))
		return false;
	// If a house
	if (alpha > 0.0 && !float_equality<double>(alpha, test_alpha, 1e-5))
		return false;
	if (beta_ih > 0.0 && !float_equality<double>(beta_ih, test_beta_ih, 1e-5))
		return false;
	// If a school or workplace
	if (psi > 0.0 && !float_equality<double>(psi, test_psi, 1e-5))
		return false;

	// If a hospital
	if (hospital_betas.size() > 0){
		if (!float_equality<double>(hospital_betas.at(std::string("hospital employee")), test_beta_employee, 1e-5))
			return false;
		if (!float_equality<double>(hospital_betas.at("hospital non-COVID patient"), test_beta_patient, 1e-5))
			return false;
		if (!float_equality<double>(hospital_betas.at("hospital testee"), test_beta_testee, 1e-5))
			return false;
	}

	//
	// Test with agents
	//
	
	std::vector<int> agents = {909, 1, 10005};
	std::vector<bool> infected = {true, false, false};
	// Hardcoded number of infected
	ninf = 1;
	ntot = 3;
	for (int i=0; i<agents.size(); ++i)
		place.register_agent(agents.at(i), infected.at(i));

	// Check if correctly registered (hardcoded)
	std::vector<int> reg_agents = place.get_agent_IDs();
	if (agents != reg_agents)
		return false;			

	// Number of infected agents
	if (ninf != place.get_total_infected())
		return false;

	//
	// Test adding and removing specific agents
	//
	
	// Addition
	int index = 201;
	place.add_agent(index);
	agents.push_back(index);
	
	reg_agents = place.get_agent_IDs();
	if (agents != reg_agents)
		return false;	

	// Removal
	index = 1;
	std::vector<int> new_agents = {909, 10005, 201};
	place.remove_agent(index);

	reg_agents = place.get_agent_IDs();
	if (new_agents != reg_agents)
		return false;

	return true;
}

/** 
 * \brief Test for household contribution to infection probability
 * 
 * @param household - household object
 * @param inf_var - infection variability
 * @param n_exp - number of exposed
 * @param n_sym - number of symptomatic
 * @param n_tot - total number of agents
 * @param exp_lambda - expected lambda
 */
bool household_contribution_test(Household& household, double inf_var, 
				int n_exp, int n_sym, int n_tot, double exp_lambda)
{
	double lambda = 0.0;

	for (int i=0; i<n_tot; ++i)
		household.add_agent(i+1);

	for (int i=0; i<(n_exp + n_sym); ++i){
		if (i < n_exp)
			household.add_exposed(inf_var);
		else if (i >= n_exp)
			household.add_symptomatic(inf_var);	
	}

	household.add_symptomatic_home_isolated(inf_var);
	household.compute_infected_contribution();

	lambda = household.get_infected_contribution();
	if (!float_equality<double>(exp_lambda, lambda, 1e-3))
		return false;

	return true;
}


