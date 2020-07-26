#ifndef INFECTION_H
#define INFECTION_H

#include "common.h"
#include "rng.h"
#include <tuple>

class RNG;

/***************************************************** 
 * class: Infection
 * 
 * Attributes and functionality of infection and its
 * transmission
 * 
 *****************************************************/

class Infection{
public:

	//
	// Constructors
	//

	/**
	 * \brief Creates an Infection object with default attributes
	 */
	Infection() = default;

	/**
	 * \brief Creates an Infection object with custom attributes
	 *
	 * @param del_t - time step 
	 */
	Infection(const double del_t) : dt(del_t) { }

	//
	// Infection transmission
	//
	
	/// \brief Compute if agent got infected
	/// @param lambda - probability factor
	bool infected(const double lambda);

	/// \brief Get latency period from a distribution
	double latency();

	/// \brief Get infection variability from a distribution
	double inf_variability();

	/// \brief Determine if the exposed agent will recover without symptoms
	bool recovering_exposed();

	/// \brief Determine if the agent will die based on agents age and total rate
	bool will_die(const int age);
	
	/// \brief Returns randomly chosen time left for agent to live
	double time_to_death();

	/// \brief Returns random house ID  
	/// @param n_hs - total number of households 
	int get_random_household_ID(const int n_hs);

	//
	// Setters
	//

	void set_latency_distribution(const double mean, const double std)
		{ ln_mean_lat = mean; ln_std_lat = std; }

	void set_inf_variability_distribution(const double k, const double theta)
		{ inf_var_k = k; inf_var_theta = theta; }

	void set_onset_to_death_distribution(const double mean, const double std)
		{ otd_mean = mean; otd_std = std; }

	/**
	 * \brief Assing various single number probabilities
	 * @param pr_e_rec - probability that exposed will recover without symptoms
	 * @param pr_dth_icu - probability of death in ICU
	 */
	void set_other_probabilities(const double pr_e_rec)
		{prob_recovering_exposed = pr_e_rec; prob_sym = 1 - prob_recovering_exposed; }

	/**
	 * \brief Process and store the age-dependent mortality rate distribution
	 * @param raw_rates - map with string of age range (inclusive) to probability 
	 */
	void set_mortality_rates(const std::map<std::string, double> raw_rates);


	//
	// Getters
	//

	/// Return map with mortality rates
	const std::map<std::string, std::tuple<int, int, double>>& get_mortality_rates() const 
		{ return mortality_rates; }

	//
	// I/O
	//

	/**
	 * \brief Print infection parameters
	 * \details The parameters are in the same order as in the constructor
     * It prints recovery time in days, not the mu_rec which is its' inverse
	 * @param where - output stream
	 */	
	void print_basic(std::ostream& where) const;

protected:
	
	//
	// Modeling parameters
	//

	// Time step
	double dt = 1.0;

	//
	// Distribution parameters
	//
	
	// Latency
	double ln_mean_lat = 0.0;
   	double ln_std_lat = 0.0;	

	// Infectiousness variability factor
	double inf_var_k = 0.0;
	double inf_var_theta = 0.0;

	// Onset to death log-normal
	double otd_mean = 0.0;
	double otd_std = 0.0;

	// Onset to hospitalization - gamma 
	double oth_k = 0.0;
	double oth_theta = 0.0;

	// Hospitalization to death - weibull
	double htd_k = 0.0;
	double htd_theta = 0.0;
	
	//
	// Other probabilities
	//
	
	// Probability that exposed will recover 
	// without developing symptoms
	double prob_recovering_exposed = 0.0; 

	// Probability of dying in ICU
	double prob_death_icu = 0.0;

	// Probability the agent will develop symptoms
	double prob_sym = 0.0;

	// Random distribution generator
	RNG rng;
	
	//
	// Age-dependent distributions
	//
	
	// Mortality rates (age group: min age, max age, probability)
	std::map<std::string, std::tuple<int, int, double>> mortality_rates;

	//
	// Private functions
	//

	// Extract min and max age in a group from age-dependent distributions
	std::vector<int> parse_age_group(const std::string group_range);
};

/// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Infection& infection);

#endif 
