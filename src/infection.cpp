#include "../include/infection.h"

/***************************************************** 
 * class: Infection
 * 
 * Attributes and functionality of infection and its
 * transmission
 * 
 *****************************************************/

//
// I/O
//

// Print infection parameters	
void Infection::print_basic(std::ostream& where) const
{
	where << dt << " " << ln_mean_lat << " " << ln_std_lat
		  << " " << inf_var_k << " " << inf_var_theta << " " 
		  << prob_recovering_exposed;
}

//
// Infection transmission
//

// Get latency period from a distribution
double Infection::latency()
{
	return rng.get_random_lognormal(ln_mean_lat, ln_std_lat);
}

// Get infection variability from a distribution
double Infection::inf_variability()
{
	return rng.get_random_gamma(inf_var_k, inf_var_theta); 
}

// Determine if exposed will go directly to recovered
bool Infection::recovering_exposed()
{
	return (rng.get_random(0, 1) <= prob_recovering_exposed);
}

// Determine if agent will die 
bool Infection::will_die(const int age)
{
	double tot_prob = 0.0;
	// Probability of death 
	for (const auto& mrt : mortality_rates){
		if ( age >= std::get<0>(mrt.second) &&
			 age <= std::get<1>(mrt.second)){
			tot_prob = std::get<2>(mrt.second);
		}
	}
	// true if going to die
	if (rng.get_random(0.0, 1.0) <= tot_prob)
		return true;
	else
		return false;	
}


// Determine time to death
double Infection::time_to_death()
{
	return rng.get_random_lognormal(otd_mean, otd_std);
}

// Returns random household ID for testing
int Infection::get_random_household_ID(int n_hs)
{
	return rng.get_random_int(1, n_hs);
}

// Returns random household ID for testing
int Infection::get_random_agent_ID(const int n_ag)
{
    return rng.get_random_int(1, n_ag);
}


//
// Setters
//

// Process and store the age-dependent mortality rate distribution
void Infection::set_mortality_rates(const std::map<std::string, double> raw_rates)
{
	std::vector<int> ages = {0,0};

	for (const auto& rr : raw_rates){
		ages = parse_age_group(rr.first);
		mortality_rates[rr.first] = std::make_tuple(ages[0], ages[1], rr.second);
	}
}

//
// Private functions
//

// Extract min and max age in a group from age-dependent distributions
std::vector<int> Infection::parse_age_group(const std::string group_range)
{
	int age = 0;
	std::vector<int> ages;

	std::istringstream group(group_range);
	std::string token;
	
	// Assumes only two numbers, first being min age
	while(std::getline(group, token, '-')){
		std::istringstream(token) >> age;
   		ages.push_back(age);
	}
	return ages;
}

// Compute if agent got infected
bool Infection::infected(const double lambda)
{
	// Probability of infection
	double prob = 1 - std::exp(-dt*lambda);

	// true if infected
	if (rng.get_random(0.0, 1.0) <= prob)
		return true;
	else
		return false; 
}

//
// Supporting functions
//

// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Infection& infection)
{
	infection.print_basic(out);
	return out;
}
