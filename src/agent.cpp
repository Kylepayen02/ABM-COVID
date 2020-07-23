#include "../include/agent.h"

/***************************************************** 
 * class: Agent
 * 
 * Defines and stores attributes of a single agent
 * 
 *****************************************************/

//
// I/O
//

// Print Agent information 
void Agent::print_basic(std::ostream& where) const
{
	where << ID << " " << is_student << " " << is_working  
		  << " " << age << " " << x << " " << y << " "
		  << house_ID << " " << " " << school_ID
		  << " " << work_ID << " " << " " << " " << is_infected;
}

//
// Infection related computations
//

//
// Supporting functions
//

// Overloaded ostream operator for I/O
std::ostream& operator<< (std::ostream& out, const Agent& agent) 
{
	agent.print_basic(out);
	return out;
}

int Agent::get_interactions(const std::vector<Agent>& agents){
    int count = 0;
    for (const Agent& agent : agents){
        if (ID == agent.get_ID())
            continue;
        // If agent is not removed by death
        if (!agent.get_dead()){
            if (house_ID == agent.get_household_ID()
                || work_ID == agent.get_work_ID()
                || school_ID == agent.get_school_ID())
            {
                count += 1;
            }
        }
    }
    return count;
}