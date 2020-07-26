#include "infection_tests.h"

/***************************************************** 
 *
 * Test suite for functionality of the Infection class
 *
 *****************************************************/

// Aliases for pointers to member functions used in these
// tests
using dist_sampling = double (Infection::*)();
using age_rates_setter = void (Infection::*)(const std::map<std::string, double>);
using age_rates_getter = const std::map<std::string, std::tuple<int, int, double>>& (Infection::*)() const;
using age_rates_caller = bool (Infection::*)(int);

// Tests
bool infection_transmission_test();
bool infection_out_test();

// Supporting functions
bool check_mortality_rates(Infection&);
bool check_age_dependent_rates(age_rates_setter, age_rates_getter, age_rates_caller,
								Infection&,	std::map<std::string, double>, 
								std::map<std::string, std::tuple<int, int, double>>);
bool check_distribution(dist_sampling, Infection&, double);
bool check_random_ID(Infection&);

int main()
{
	test_pass(infection_transmission_test(), "Infection class transmission functionality");
	test_pass(infection_out_test(), "Infection class ostream operator");
}

/// Tests functionality related to infection transmission
bool infection_transmission_test()
{
	double delta_t = 1.5;
	double mean = 5.0, std = 0.1, k = 0.2, theta = 1.2;
	double otd_mu = 2.6696, otd_sigma = 0.4760;
	double oth_k = 1.2, oth_theta = 0.1;
	double ohd_k = 5.0, ohd_theta = 1.5;
	
	// Expected mean values of distributions
	double mean_latency = 149.1571, mean_var = 0.24; 
	double mean_otd = 16.1652;
	double mean_oth = 0.12;
   	double mean_ohd = 1.3773;	

	Infection infection(delta_t);
	infection.set_latency_distribution(mean, std);
	infection.set_inf_variability_distribution(k, theta);

	infection.set_onset_to_death_distribution(otd_mu, otd_sigma);


	// Infection
	// Should be infected
	if (infection.infected(delta_t) == false)
		return false;
	// Should not be infected
	if (infection.infected(1e-16) == true)
		return false;

	// Exposed recovering without symptoms
	// and agent not dying in ICU
	//infection.set_other_probabilities(1.0, 0.0, 0.0);

	// Age-dependent rates 
	if (!check_mortality_rates(infection))
		return false;


	// Check genration of random place ID functions
	if (!check_random_ID(infection))
		return false;	

	// Check distributions
	if (!check_distribution(&Infection::latency, infection, mean_latency))
		return false;		
	if (!check_distribution(&Infection::inf_variability, infection, mean_var))
		return false;	
	if (!check_distribution(&Infection::time_to_death, infection, mean_otd))
		return false;
	
	return true;
}

/// Tests Infection ostream operator overload/print capabilities
bool infection_out_test()
{
	double delta_t = 1.5;
	double mean = 5.0, std = 0.1, k = 0.2, theta = 1.0;

	Infection infection(delta_t);
	infection.set_latency_distribution(mean, std);
	infection.set_inf_variability_distribution(k, theta);

	// Get directly from the stream and compare
	std::stringstream inf_buff;
	inf_buff << infection;
	std::istringstream res(inf_buff.str());

	double test_dt = 0.0, test_mean = 0.0, test_std = 0.0;
   	double test_k = 0.0, test_theta = 0.0;	

	res >> test_dt >> test_mean >> test_std >> test_k >> test_theta;

	if (!float_equality<double>(delta_t, test_dt, 1e-5))
		return false;

	if (!float_equality<double>(mean, test_mean, 1e-5)
			|| !float_equality<double>(std, test_std, 1e-5))
		return false;

	if (!float_equality<double>(k, test_k, 1e-5)
			|| !float_equality<double>(theta, test_theta, 1e-5))
		return false;

	return true;
}

/// Test if generated mortality rates correspond to actual
bool check_mortality_rates(Infection& infection)
{
	// Input
	std::map<std::string, double> raw_mortality_rates = {{"0-9", 0.00002},  
			{"10-19", 0.00006}, {"20-29", 0.0003}, {"30-39", 0.0008},
			{"40-49", 0.0015}, {"50-59", 0.006}, {"60-69", 0.022},
			{"70-79", 0.051}, {"80-120", 0.093}};	

	// Expected output
	std::map<std::string, std::tuple<int, int, double>> mortality_rates = 
					   {{"0-9", std::make_tuple(0, 9, 0.00002)},
						{ "10-19", std::make_tuple(10, 19, 0.00006)},
						{ "20-29", std::make_tuple(20, 29, 0.0003)},
						{ "30-39", std::make_tuple(30, 39, 0.0008)},
						{ "40-49", std::make_tuple(40, 49, 0.0015)},
						{ "50-59", std::make_tuple(50, 59, 0.006)},
						{ "60-69", std::make_tuple(60, 69, 0.022)},
						{ "70-79", std::make_tuple(70, 79, 0.051)},
						{ "80-120", std::make_tuple(80, 120, 0.093)}};

	// Verification 
	return check_age_dependent_rates(&Infection::set_mortality_rates, &Infection::get_mortality_rates,
					&Infection::will_die, infection, raw_mortality_rates, mortality_rates);
}




/// \brief Function for checking different types of age-dependent rates
bool check_age_dependent_rates(age_rates_setter set_rates, age_rates_getter get_rates,
								age_rates_caller call, Infection& infection, 
								std::map<std::string, double> raw_rates, 
								std::map<std::string, std::tuple<int, int, double>> expected_rates)
{
	// Create and retrieve rates 
	(infection.*set_rates)(raw_rates);
	const std::map<std::string, std::tuple<int, int, double>>& created_rates = (infection.*get_rates)();

	// Compare
	std::tuple<int, int, double> exp_rate;
	for (const auto& rate : created_rates){
		exp_rate = expected_rates[rate.first];
		if (std::get<0>(exp_rate) != std::get<0>(rate.second))
			return false;
		if (std::get<1>(exp_rate) != std::get<1>(rate.second))
			return false;
		if (!float_equality<double>(std::get<2>(exp_rate), std::get<2>(rate.second), 1e-5))
			return false;
	}			

	// Verify the checking and sampling for each age group
	// It samples n_tot population of a fixed age and then 
	// checks if mortality rate comparable to nominal
	int n_tot = 1e6;
	for (const auto& rate : created_rates){
		int n_affected = 0;
		int age = std::get<0>(rate.second)+1;
		for (int i=0; i<n_tot; ++i){
			if ((infection.*call)(age) == true)
				n_affected++;	
		}
		double fr_affected = (static_cast<double>(n_affected))/(static_cast<double>(n_tot));
		double fr_exp = std::get<2>(rate.second);
		if (!float_equality<double>(fr_affected, fr_exp, 0.01))		
			return false;
	}
	return true;
}

/// \brief Test sampling of an infection distribution against expected mean
bool check_distribution(dist_sampling dist, Infection& infection, double exp_mean)
{
	std::vector<double> rnum;
	for (int i=0; i<1000000; ++i)
		rnum.push_back((infection.*dist)());
	
	// Compare mean value
	double rng_mean = std::accumulate(rnum.begin(), rnum.end(), 0.0)/(static_cast<double>(rnum.size()));
	
	if (!float_equality<double>(exp_mean, rng_mean, 0.01))
		return false;
	
	return true;
}

/// \brief Verification of functionality for generating random IDs
bool check_random_ID(Infection& infection)
{
	// Increase index of a place with a randomly
	// generated ID - at the end all the places should
	// have associated values larger than 0

	std::vector<int> houses = {0, 0, 0};
	std::vector<int> hospitals = {0, 0, 0, 0};
	
	int n_hs = houses.size();
	int n_hsp = hospitals.size();

	int n_tot = 1000;
	for (int i=0; i<n_tot; ++i){
		++houses.at(infection.get_random_household_ID(n_hs)-1);
	}

	if (std::find(houses.begin(), houses.end(), 0) != houses.end())
		return false;

	return true;
}


