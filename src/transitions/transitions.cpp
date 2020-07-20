#include "../../include/transitions/transitions.h"

/***************************************************** 
 * class: Transitions 
 *
 * Interface for computing of transitioning 
 * between different agents states 
 * 
 ******************************************************/

// Implement transitions relevant to susceptible
int Transitions::susceptible_transitions(Agent& agent, const double time, 
				const double dt, Infection& infection,	
				std::vector<Household>& households, std::vector<School>& schools,
				std::vector<Workplace>& workplaces,const std::map<std::string,
				double>& infection_parameters, std::vector<Agent>& agents)
{
    bool got_infected_reg = false;
	int got_infected = 0;

    got_infected = regular_tr.susceptible_transitions(agent, time, infection,
            households, schools, workplaces,infection_parameters, agents);
    got_infected_reg = static_cast<bool>(got_infected);

    got_infected = static_cast<int>(got_infected_reg);

    return got_infected;

}

// Implement transitions relevant to exposed 
int Transitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces,
										const std::map<std::string, double>& infection_parameters)
{
	bool recovered_reg = false;
	int agent_recovered = 0;

    agent_recovered = regular_tr.exposed_transitions(agent, infection, time, dt,
                households, schools, workplaces, infection_parameters);
    recovered_reg = static_cast<bool>(agent_recovered);

	agent_recovered = static_cast<int>(recovered_reg);

	return agent_recovered;
}

// Transitions of a symptomatic agent 
std::vector<int> Transitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces,
					const std::map<std::string, double>& infection_parameters)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	std::vector<bool> removed_reg = {0,0};


    removed = regular_tr.symptomatic_transitions(agent, time, dt, infection,
                households, schools, workplaces, infection_parameters);
    removed_reg = {static_cast<bool>(removed.at(0)), static_cast<bool>(removed.at(1))};


	removed.at(0) = static_cast<int>(removed_reg.at(0));
	removed.at(1) = static_cast<int>(removed_reg.at(1));

	return removed;
}


