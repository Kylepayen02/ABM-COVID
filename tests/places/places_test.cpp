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
    double severity_cor = 2.0, beta_emp = 0.9, beta = 0.47;
    double psi_emp = 0.1, psi_s = 0.25;

    School school(pID, xi, yi, severity_cor, psi_emp, psi_s, beta_emp, beta);

    int ntot = 0, ninf = 0;
    int test_pID = 0, test_ntot = 0, test_ninf = 0;

    int test_ID = 0;
    double test_xi = 0.0, test_yi = 0.0;
    double test_ck = 0.0, test_beta_emp = 0.0, test_beta = 0.0;
    double test_psi_emp = 0.0, test_psi_s = 0.0;

    std::stringstream place_buff;
    place_buff << school;
    std::istringstream res(place_buff.str());

    res >> test_pID >> test_xi >> test_yi >> test_ntot >> test_ninf >> test_ck >> test_beta >> test_beta_emp >> test_psi_emp >> test_psi_s;

    // Check each value
    if (pID != test_pID || ntot != test_ntot || ninf != test_ninf)
        return false;
    if (!float_equality<double>(xi, test_xi, 1e-5))
        return false;
    if (!float_equality<double>(yi, test_yi, 1e-5))
        return false;
    if (!float_equality<double>(severity_cor, test_ck, 1e-5))
        return false;
    if ( !float_equality<double>(beta_emp, test_beta_emp, 1e-5) || !float_equality<double>(beta, test_beta, 1e-5))
        return false;
    if (!float_equality<double>(psi_emp, test_psi_emp, 1e-5) && !float_equality<double>(psi_s, test_psi_s, 1e-5))
        return false;

    //
    // Test with agents
    //

    std::vector<int> agents = {909, 1, 10005};
    std::vector<bool> infected = {true, false, false};
    // Hardcoded number of infected
    ninf = 1;
    ntot = 3;
    for (int i=0; i<agents.size(); ++i)
        school.register_agent(agents.at(i), infected.at(i));

    // Check if correctly registered (hardcoded)
    std::vector<int> reg_agents = school.get_agent_IDs();
    if (agents != reg_agents)
        return false;

    // Number of infected agents
    if (ninf != school.get_total_infected())
        return false;

    //
    // Test adding and removing specific agents
    //

    // Addition
    int index = 201;
    school.add_agent(index);
    agents.push_back(index);

    reg_agents = school.get_agent_IDs();
    if (agents != reg_agents)
        return false;

    // Removal
    index = 1;
    std::vector<int> new_agents = {909, 10005, 201};
    school.remove_agent(index);

    reg_agents = school.get_agent_IDs();
    if (new_agents != reg_agents)
        return false;

    return true;
}


/// Test contribution computation
bool contribution_test_school()
{
    int pID = 130, type = 2;
    double xi = 0.05, yi = 0.134;
    double severity_cor = 2.0, beta_emp = 0.1, beta = 0.4;
    double psi_emp = 0.3, psi_s = 0.25, inf_var = 0.9;

    int n_exp = 3, n_sym = 6, n_tot = 20;
    int n_emp_exp = 2, n_emp_sym = 3;
    double exp_lambda = 0.1251;

    School school(pID, xi, yi, severity_cor, psi_emp, psi_s, beta_emp, beta);

    double lambda = 0.0;

    for (int i=0; i<n_tot; ++i)
        school.add_agent(i+1);

    // Students
    for (int i=0; i<(n_exp + n_sym); ++i){
        if (i < n_exp)
            school.add_exposed(inf_var);
        else if (i >= n_exp)
            school.add_symptomatic_student(inf_var);
    }

    // Employees
    for (int i=0; i<(n_emp_exp + n_emp_sym); ++i){
        if (i < n_emp_exp)
            school.add_exposed_employee(inf_var);
        else if (i >= n_emp_exp)
            school.add_symptomatic_employee(inf_var);
    }

    school.compute_infected_contribution();

    lambda = school.get_infected_contribution();
    if (!float_equality<double>(exp_lambda, lambda, 1e-3)){
        return false;
    }
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



/// Tests all public functions from the Household class  
bool household_test()
{
	int pID = 176;
	double xi = 5.95671, yi = 11.00675;
	double severity_cor = 3.5, beta = 0.90, beta_ih = 0.4;
	double alpha = 0.7;

	Household house(pID, xi, yi, alpha, severity_cor, beta);

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

	Household house(pID, xi, yi, alpha, severity_cor, beta);

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
	// If a workplace
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


