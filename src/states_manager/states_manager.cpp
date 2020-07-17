#include "../../include/states_manager/states_manager.h"

/***************************************************** 
 * class: StatesManager 
 *
 * Functionality for modifying agents states  
 * 
 ******************************************************/

// Set all states for transition from susceptible to exposed
void StatesManager::set_susceptible_to_exposed(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(true);

	agent.set_recovering_exposed(false);	
	agent.set_symptomatic(false);

	
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}

// Set all states for transition from susceptible to exposed that will never become symptomatic
void StatesManager::set_susceptible_to_exposed_never_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(true);
	agent.set_recovering_exposed(true);	

	agent.set_symptomatic(false);

	
	agent.set_dying(false);
	agent.set_recovering(true);	
	agent.set_removed(false);
}

// Set exposed that never developed symptoms to removed
void StatesManager::set_exposed_never_symptomatic_to_removed(Agent& agent)
{
	set_any_to_removed(agent);
}

// Set all states for transition from exposed to general symptomatic
void StatesManager::set_exposed_to_symptomatic(Agent& agent)
{
	agent.set_infected(true);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	

	agent.set_symptomatic(true);

	
	agent.set_dying(false);
	agent.set_recovering(false);	
	agent.set_removed(false);
}



// Set all states relevant to agent that will die 
void StatesManager::set_dying_symptomatic(Agent& agent)
{
	agent.set_dying(true);
	agent.set_recovering(false);	
}

// Set all states relevant to agent that will recover 
void StatesManager::set_recovering_symptomatic(Agent& agent)
{
	agent.set_dying(false);
	agent.set_recovering(true);	
}





// Set all removed related states
void StatesManager::set_any_to_removed(Agent& agent)
{
	agent.set_removed(true);

	agent.set_dying(false);
	agent.set_recovering(false);	

	agent.set_infected(false);
	agent.set_exposed(false);
	agent.set_recovering_exposed(false);	

	agent.set_symptomatic(false);
	

}


