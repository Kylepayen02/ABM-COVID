#include "../../include/transitions/regular_transitions.h"

/***************************************************** 
 * class: RegularTransitions 
 *
 * Functionality for computing of transitioning 
 * between different agents states for a most general
 * case of agent 
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
int RegularTransitions::susceptible_transitions(Agent& agent, const double time, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,
				const std::map<std::string, double>& infection_parameters, std::vector<Agent>& agents)
{
	double lambda_tot = 0.0;
	int got_infected = 0;

	lambda_tot = compute_susceptible_lambda(agent, time, households, schools, workplaces);
	if (infection.infected(lambda_tot) == true){
		got_infected = 1;
		agent.set_inf_variability_factor(infection.inf_variability());
		// Infectiousness, latency, and possibility of never developing 
		// symptoms 
		recovery_and_incubation(agent, infection, time, infection_parameters);
	}
	return got_infected;	
}

// Return total lambda of susceptible agent 
double RegularTransitions::compute_susceptible_lambda(const Agent& agent, const double time, 
					const std::vector<Household>& households, const std::vector<School>& schools,
					const std::vector<Workplace>& workplaces)			
{
	double lambda_tot = 0.0;

	// Regular susceptible agent
	const Household& house = households.at(agent.get_household_ID()-1);
	if (agent.student() && agent.works()){
		const School& school = schools.at(agent.get_school_ID()-1);
		const Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						workplace.get_infected_contribution()+
						school.get_infected_contribution();
	} else if (agent.student()){
		const School& school = schools.at(agent.get_school_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						school.get_infected_contribution();
	} else if (agent.works()){
		const Workplace& workplace = workplaces.at(agent.get_work_ID()-1);
		lambda_tot = house.get_infected_contribution()+ 
						workplace.get_infected_contribution();
	} else {
		lambda_tot = house.get_infected_contribution();
	}	

	return lambda_tot;
}

// Compute and set agent properties related to recovery without symptoms and incubation
void RegularTransitions::recovery_and_incubation(Agent& agent, Infection& infection, const double time,
				const std::map<std::string, double>& infection_parameters)
{
	// Determine if agent will recover without
	// becoming symptomatic and update corresponding states
	bool never_sy = infection.recovering_exposed();

	// Total latency period
	double latency = infection.latency();
	// Portion of latency when the agent is not infectious
	double dt_ninf = std::min(infection_parameters.at("time from exposed to infectiousness"), latency);

	if (never_sy){
		states_manager.set_susceptible_to_exposed_never_symptomatic(agent);
		// Set to total latency + infectiousness duration
		double rec_time = infection_parameters.at("recovery time");
		agent.set_latency_duration(latency + rec_time);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}else{
		// If latency shorter, then  not infectious during the entire latency
		states_manager.set_susceptible_to_exposed(agent);
		agent.set_latency_duration(latency);
		agent.set_latency_end_time(time);
		agent.set_infectiousness_start_time(time, dt_ninf);
	}
}

// Implement transitions relevant to exposed 
int RegularTransitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces,
										const std::map<std::string, double>& infection_parameters)
{

	// Check if latency time is over
	int agent_recovered = 0;
	if (agent.get_latency_end_time() <= time){
		// Recovering without symptoms - remove
		if (agent.recovering_exposed()){
			states_manager.set_exposed_never_symptomatic_to_removed(agent);
			agent_recovered = 1;
		} else {
			// Transition to symptomatic
			states_manager.set_exposed_to_symptomatic(agent);
			// Removal settings
			int agent_age = agent.get_age();
			if (infection.will_die_non_icu(agent_age)){
				states_manager.set_dying_symptomatic(agent);			
				agent.set_time_to_death(infection.time_to_death());
				agent.set_death_time(time);
			} else {
				states_manager.set_recovering_symptomatic(agent);			
				// This may change if treatment is ICU
				agent.set_recovery_duration(infection_parameters.at("recovery time"));
				agent.set_recovery_time(time);		
			}
		}
	}
	return agent_recovered;
}

// Transitions of a symptomatic agent 
std::vector<int> RegularTransitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces,
					const std::map<std::string, double>& infection_parameters)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	removed = check_agent_removal(agent, time, households, schools, workplaces);
	if (agent.removed() == true)
		return removed;
}

// Verify if agent is to be removed at this step
std::vector<int> RegularTransitions::check_agent_removal(Agent& agent, const double time,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	// If dying
	if (agent.dying() == true){
		if (agent.get_time_of_death() <= time){
			removed.at(1) = 1;
			remove_agent_from_all_places(agent, households, schools, workplaces);
			states_manager.set_any_to_removed(agent);
		}
	}
	// If recovering
	if (agent.recovering() == true){
		if (agent.get_recovery_time() <= time){
			removed.at(0) = 1;
			states_manager.set_any_to_removed(agent);
		}
	}
	return removed;
}

// Remove agent's ID from places where they are registered
void RegularTransitions::remove_agent_from_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces)
{
	// No error if already removed due to hospitalization
	int agent_ID = agent.get_ID();

	int hs_ID = agent.get_household_ID();
	if (hs_ID > 0){
		households.at(hs_ID-1).remove_agent(agent_ID);
	} else {
		throw std::runtime_error("Regular symptomatic agent does not have a valid household ID");
	}
	if (agent.student())
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
}

// Add to all from places where they are registered
void RegularTransitions::add_to_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).add_agent(agent_ID);
}

// Remove agent's index from all workplaces and schools that have them registered
void RegularTransitions::remove_from_all_workplaces_and_schools(const Agent& agent,
							std::vector<School>& schools, std::vector<Workplace>& workplaces)
{
	int agent_ID = agent.get_ID();
	if (agent.student())
		schools.at(agent.get_school_ID()-1).remove_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).remove_agent(agent_ID);
}

// Add agent's ID back to the places where they are registered
// This is done to keep realistic numbers of people in different places
// which influences the probability
void RegularTransitions::add_agent_to_all_places(const Agent& agent, 
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces)
{
	int agent_ID = agent.get_ID();
		
	if (agent.student())
		schools.at(agent.get_school_ID()-1).add_agent(agent_ID);
	if (agent.works())
		workplaces.at(agent.get_work_ID()-1).add_agent(agent_ID);
}


