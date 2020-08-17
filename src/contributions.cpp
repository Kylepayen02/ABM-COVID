#include "../include/contributions.h"

/***************************************************** 
 * class: Contributions
 *
 * Functionality for computing of infection probability
 * contributions from agents to places and eventually 
 * mobility 
 * 
 ******************************************************/

// Count contributions of an exposed agent
void Contributions::compute_exposed_contributions(const Agent& agent, const double time,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces)
{
	// Skip if not yet infectious
	if (time < agent.get_infectiousness_start_time()){
		return;
	}
	
	// Agent's infection variability
	double inf_var = 0.0;
	inf_var = agent.get_inf_variability_factor();

    // Household
    Household& household = households.at(agent.get_household_ID()-1);
    household.add_exposed(inf_var);

    // Other places
    if (agent.student() == true){
        School& school = schools.at(agent.get_school_ID()-1);
        school.add_exposed(inf_var);
    }
    if (agent.works() == true){
        if (agent.school_employee()){
            School& sch = schools.at(agent.get_work_ID()-1);
            sch.add_exposed_employee(inf_var);
        } else {
            Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
            workplace.add_exposed(inf_var);
        }
    }
}

// Count contributions of a symptomatic agent
void Contributions::compute_symptomatic_contributions(const Agent& agent, const double time,	
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces)
{
	// Agent's infection variability
	double inf_var = 0.0;
	inf_var = agent.get_inf_variability_factor();

    // Household
    Household& household = households.at(agent.get_household_ID()-1);
    household.add_symptomatic(inf_var);

    // Other places
    if (agent.student() == true){
        School& school = schools.at(agent.get_school_ID()-1);
        school.add_symptomatic(inf_var);
    }
    if (agent.works() == true){
        if (agent.school_employee()){
            School& sch = schools.at(agent.get_work_ID()-1);
            sch.add_symptomatic_employee(inf_var);
        } else {
            Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
            workplace.add_symptomatic(inf_var);
        }
    }
}

// Compute the total contribution to infection probability at every place
void Contributions::total_place_contributions(std::vector<Household>& households, 
					std::vector<School>& schools, std::vector<Workplace>& workplaces)
{	
	auto infected_contribution = [](Place& place){ place.compute_infected_contribution(); };

	std::for_each(households.begin(), households.end(), infected_contribution);
	std::for_each(schools.begin(), schools.end(), infected_contribution);
	std::for_each(workplaces.begin(), workplaces.end(), infected_contribution);
}

/// \brief Set contributions/sums from all agents in places to 0.0 
void Contributions::reset_sums(std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces)
{
	auto reset_contributions = [](Place& place){ place.reset_contributions(); };

	std::for_each(households.begin(), households.end(), reset_contributions);
	std::for_each(schools.begin(), schools.end(), reset_contributions);
	std::for_each(workplaces.begin(), workplaces.end(), reset_contributions);
}


