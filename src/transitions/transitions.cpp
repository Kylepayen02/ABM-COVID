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
				std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
				const std::map<std::string, double>& infection_parameters, std::vector<Agent>& agents, Flu& flu)
{
	bool got_infected_flu = false, got_infected_hsp_emp = false;
	bool got_infected_hsp_pt = false, got_infected_reg = false;
	int got_infected = 0;

	if (agent.symptomatic_non_covid()){
		got_infected = flu_tr.susceptible_transitions(agent, time, infection,
				households, schools, workplaces, hospitals, 
				infection_parameters, agents, flu, dt);
		got_infected_flu = static_cast<bool>(got_infected);
	} else if (agent.hospital_employee()){
		got_infected = hsp_emp_tr.susceptible_transitions(agent, time, infection,
				households, schools, hospitals,	infection_parameters, agents);
		got_infected_hsp_emp = static_cast<bool>(got_infected);
	} else if (agent.hospital_non_covid_patient()){
		got_infected = hsp_pt_tr.susceptible_transitions(agent, time, infection,
				hospitals, infection_parameters, agents);
		got_infected_hsp_pt = static_cast<bool>(got_infected);
	} else {
		got_infected = regular_tr.susceptible_transitions(agent, time, infection,
				households, schools, workplaces, hospitals, 
				infection_parameters, agents, flu);
		got_infected_reg = static_cast<bool>(got_infected);
	}

	got_infected = static_cast<int>(got_infected_flu || got_infected_hsp_emp 
					|| got_infected_hsp_pt || got_infected_reg);
	return got_infected;	
}

// Implement transitions relevant to exposed 
int Transitions::exposed_transitions(Agent& agent, Infection& infection, const double time, const double dt, 
										std::vector<Household>& households, std::vector<School>& schools,
										std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
										const std::map<std::string, double>& infection_parameters)
{
	bool recovered_hsp_emp = false, recovered_hsp_pt = false, recovered_reg = false;
	int agent_recovered = 0;

	if (agent.hospital_employee()){
		agent_recovered = hsp_emp_tr.exposed_transitions(agent, infection, time, dt, 
					households, schools, hospitals,	infection_parameters);
		recovered_hsp_emp = static_cast<bool>(agent_recovered);
	} else if (agent.hospital_non_covid_patient()){
		agent_recovered = hsp_pt_tr.exposed_transitions(agent, infection, time, dt, 
					households, hospitals, infection_parameters);
		recovered_hsp_pt = static_cast<bool>(agent_recovered);
	} else {
		agent_recovered = regular_tr.exposed_transitions(agent, infection, time, dt, 
					households, schools, workplaces, hospitals, infection_parameters);
		recovered_reg = static_cast<bool>(agent_recovered);
	}
	
	agent_recovered = static_cast<int>(recovered_hsp_emp || recovered_hsp_pt || recovered_reg);

	return agent_recovered;
}

// Transitions of a symptomatic agent 
std::vector<int> Transitions::symptomatic_transitions(Agent& agent, const double time, 
				   	const double dt, Infection& infection,
					std::vector<Household>& households, std::vector<School>& schools,
					std::vector<Workplace>& workplaces, std::vector<Hospital>& hospitals,
					const std::map<std::string, double>& infection_parameters)
{
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};
	std::vector<bool> removed_hsp_emp = {0,0}, removed_hsp_pt = {0,0}, removed_reg = {0,0};

	if (agent.hospital_employee()){
		removed = hsp_emp_tr.symptomatic_transitions(agent, time, dt, infection,  
					households, schools, hospitals, infection_parameters);
		removed_hsp_emp = {static_cast<bool>(removed.at(0)), static_cast<bool>(removed.at(1))};
	} else if (agent.hospital_non_covid_patient()){
		removed = hsp_pt_tr.symptomatic_transitions(agent, time, dt, 
					infection, households, hospitals, infection_parameters);
		removed_hsp_pt = {static_cast<bool>(removed.at(0)), static_cast<bool>(removed.at(1))};
	} else {
		removed = regular_tr.symptomatic_transitions(agent, time, dt, infection, 
					households, schools, workplaces, hospitals, infection_parameters);
		removed_reg = {static_cast<bool>(removed.at(0)), static_cast<bool>(removed.at(1))};
	}

	removed.at(0) = static_cast<int>(removed_hsp_emp.at(0) || removed_hsp_pt.at(0)
						|| removed_reg.at(0)); 
	removed.at(1) = static_cast<int>(removed_hsp_emp.at(1) || removed_hsp_pt.at(1)
						|| removed_reg.at(1));

	return removed;
}


