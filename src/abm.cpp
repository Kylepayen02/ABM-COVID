#include "../include/abm.h"

/***************************************************** 
 * class: ABM
 * 
 * Interface for agent-based modeling 
 *
 * Provides operations for creation, management, and
 * progression of an agent-based model
 *
 * Stores model-related data
 *
 * NOTE: IDs of objects correspond to their positions
 * in the vectors of objects and determine the way
 * they are accessed; IDs start with 1 but are corrected
 * by -1 when accessing;   
 * 
******************************************************/

//
// Initialization and object construction
//

// Load infection parameters, store in a map
void ABM::load_infection_parameters(const std::string infile)
{
	// Load parameters
	LoadParameters ldparam;
	infection_parameters = ldparam.load_parameter_map(infile);

	// Set infection distributions
	infection.set_latency_distribution(infection_parameters.at("latency log-normal mean"),
					infection_parameters.at("latency log-normal standard deviation"));	
	infection.set_inf_variability_distribution(infection_parameters.at("agent variability gamma shape"),
					infection_parameters.at("agent variability gamma scale"));
	infection.set_onset_to_death_distribution(infection_parameters.at("otd logn mean"), 
					infection_parameters.at("otd logn std"));
	// Set single-number probabilities
	infection.set_other_probabilities(infection_parameters.at("fraction exposed never symptomatic"));
}

// Load age-dependent distributions, store in a map of maps
void ABM::load_age_dependent_distributions(const std::map<std::string, std::string> dist_files)
{
	LoadParameters ldparam;
	std::map<std::string, double> one_file;
	for (const auto& dfile : dist_files){
		one_file = ldparam.load_age_dependent(dfile.second);
		for (const auto& entry : one_file){
			age_dependent_distributions[dfile.first][entry.first] = entry.second;
		}
		one_file.clear();
	}

	// Send to Infection class for further processing 
	infection.set_mortality_rates(age_dependent_distributions.at("mortality"));
}

// Generate and store household objects
void ABM::create_households(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One household per line
	for (auto& house : file){
		// Extract properties, add infection parameters
		Household temp_house(std::stoi(house.at(0)), 
			std::stod(house.at(1)), std::stod(house.at(2)),
			infection_parameters.at("household scaling parameter"),
			infection_parameters.at("severity correction"),
			infection_parameters.at("household transmission rate"));
		// Store 
		households.push_back(temp_house);
	}
}

// Generate and store school objects
void ABM::create_schools(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One workplace per line
	for (auto& school : file){
		// Extract properties, add infection parameters

		// School-type dependent absenteeism
		double psi = 0.0;
		std::string school_type = school.at(3);
		if (school_type == "daycare")
 			psi = infection_parameters.at("daycare absenteeism correction");
		else if (school_type == "primary" || school_type == "middle")
 			psi = infection_parameters.at("primary and middle school absenteeism correction");
		else if (school_type == "high")
 			psi = infection_parameters.at("high school absenteeism correction");
		else if (school_type == "college")
 			psi = infection_parameters.at("college absenteeism correction");
		else
			throw std::invalid_argument("Wrong school type: " + school_type);

        School temp_school(std::stoi(school.at(0)),
                           std::stod(school.at(1)), std::stod(school.at(2)),
                           infection_parameters.at("severity correction"),
                           infection_parameters.at("school employee absenteeism correction"), psi,
                           infection_parameters.at("school employee transmission rate"),
                           infection_parameters.at("school transmission rate"));

		// Store 
		schools.push_back(temp_school);
	}
}

// Generate and store workplace objects
void ABM::create_workplaces(const std::string fname)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);
	
	// One workplace per line
	for (auto& work : file){
		// Extract properties, add infection parameters
		Workplace temp_work(std::stoi(work.at(0)), 
			std::stod(work.at(1)), std::stod(work.at(2)),
			infection_parameters.at("severity correction"),
			infection_parameters.at("work absenteeism correction"),
			infection_parameters.at("workplace transmission rate"));

		// Store 
		workplaces.push_back(temp_work);
	}
}


// Create agents and assign them to appropriate places
void ABM::create_agents(const std::string fname, const int ninf0)
{
    load_agents(fname, ninf0);
    register_agents();
}

// Retrieve agent information from a file
void ABM::load_agents(const std::string fname, const int ninf0)
{
	// Read the whole file
	std::vector<std::vector<std::string>> file = read_object(fname);

    // For custom generation of initially infected
    std::vector<int> infected_IDs(ninf0);
    if (ninf0 != 0){
        int nIDs = file.size();
        // Random choice of IDs
        for (int i=0; i<ninf0; ++i){
            infected_IDs.at(i) = infection.get_random_agent_ID(nIDs);
        }
    }

	// Counter for agent IDs
	int agent_ID = 1;

	// One agent per line, with properties as defined in the line
	for (auto agent : file){
		// Agent status
		bool student = false, works = false, worksSch = false;
		int house_ID = -1;


		house_ID = std::stoi(agent.at(5));

		// Check if agent is a student
		if (std::stoi(agent.at(0)) == 1)
			student = true;
		// Check if agents works
		if (std::stoi(agent.at(1)) == 1)
			works = true;

		// Random or from the input file
		bool infected = false;
        if (ninf0 != 0){
            auto iter = std::find(infected_IDs.begin(), infected_IDs.end(), agent_ID);
            if (iter != infected_IDs.end()){
                infected_IDs.erase(iter);
                infected = true;
                n_infected_tot++;
            }
        } else {
            if (std::stoi(agent.at(9)) == 1){
                infected = true;
                n_infected_tot++;
            }
        }

        if (std::stoi(agent.at(7)) == 1)
            worksSch = true;

		Agent temp_agent(student, works, std::stoi(agent.at(2)),
			std::stod(agent.at(3)), std::stod(agent.at(4)), house_ID,
			std::stoi(agent.at(6)), worksSch, std::stoi(agent.at(8)), infected);

		// Set Agent ID
		temp_agent.set_ID(agent_ID++);

		// Set properties for exposed if initially infected
		if (temp_agent.infected() == true)
			initial_exposed(temp_agent);

		// Store
		agents.push_back(temp_agent);
	}
}

// Assign agents to households, schools, and workplaces
void ABM::register_agents()
{
	int house_ID = 0, school_ID = 0, work_ID = 0;
	int agent_ID = 0;
	bool infected = false;

	for (auto& agent : agents){
		
		// Agent ID and infection status
		agent_ID = agent.get_ID();
		infected = agent.infected();

        // register in the household
        // Assign agent to random household

//        house_ID = infection.get_random_household_ID(households.size());
//        agent.set_household_ID(house_ID);
        house_ID = agent.get_household_ID();
        Household& house = households.at(house_ID - 1);
        house.register_agent(agent_ID, infected);


		// Register in schools and workplaces
		if (agent.student()){
			school_ID = agent.get_school_ID();
			School& school = schools.at(school_ID - 1); 
			school.register_agent(agent_ID, infected);		
		}

		if (agent.works()){
			work_ID = agent.get_work_ID();
//			Workplace& work = workplaces.at(work_ID - 1);
//			work.register_agent(agent_ID, infected);
            if (agent.school_employee()){
                School& school = schools.at(work_ID - 1);
                school.register_agent(agent_ID, infected);
            }else{
                Workplace& work = workplaces.at(work_ID - 1);
                work.register_agent(agent_ID, infected);
            }
		}
	} 
}

// Might use, might not
// Initial set-up of exposed agents
//void ABM::initial_exposed_with_never_sy(Agent& agent)
//{
//	bool never_sy = infection.recovering_exposed();
//	// Total latency period
//	double latency = infection.latency();
//	// Portion of latency when the agent is not infectious
//	double dt_ninf = std::min(infection_parameters.at("time from exposed to infectiousness"), latency);
//
//	if (never_sy){
//		// Set to total latency + infectiousness duration
//		double rec_time = infection_parameters.at("recovery time");
//		agent.set_latency_duration(latency + rec_time);
//		agent.set_latency_end_time(time);
//		agent.set_infectiousness_start_time(time, dt_ninf);
//	}else{
//		// If latency shorter, then  not infectious during the entire latency
//		agent.set_latency_duration(latency);
//		agent.set_latency_end_time(time);
//		agent.set_infectiousness_start_time(time, dt_ninf);
//	}
//	agent.set_inf_variability_factor(infection.inf_variability());
//	agent.set_exposed(true);
//	agent.set_recovering_exposed(never_sy);
//}

// Initial set-up of exposed agents
void ABM::initial_exposed(Agent& agent)
{
    bool never_sy = infection.recovering_exposed();
    // Total latency period
    double latency = infection.latency();
    // Portion of latency when the agent is not infectious
    double dt_ninf = std::min(infection_parameters.at("time from exposed to infectiousness"), latency);

    if (never_sy){
        // Set to total latency + infectiousness duration
        double rec_time = infection_parameters.at("recovery time");
        agent.set_latency_duration(latency + rec_time);
        agent.set_latency_end_time(time);
        agent.set_infectiousness_start_time(time, dt_ninf);
    }else{
        // If latency shorter, then  not infectious during the entire latency
        agent.set_latency_duration(latency);
        agent.set_latency_end_time(time);
        agent.set_infectiousness_start_time(time, dt_ninf);
    }
    agent.set_inf_variability_factor(infection.inf_variability());
    agent.set_exposed(true);
    agent.set_recovering_exposed(never_sy);
}

//
// Transmission of infection
//

/// Transmit infection according to Infection model
void ABM::transmit_infection() 
{ 
	// Compute infectious agents contributions
	// to probability sums in each place as
	// well as total place contributions
	compute_place_contributions();	

	// Determine and update state transitions
	compute_state_transitions();
	
	// Reset the place sums
	contributions.reset_sums(households, schools, workplaces);

	// Increase the time
	advance_in_time();	
}

// Count contributions of all infectious agents in each place
void ABM::compute_place_contributions()
{
	for (const auto& agent : agents){

		// Removed and susceptible don't contribute
		if (agent.removed() == true){
			continue;
		}


		if (agent.infected() == false){
			continue;
		}

		// Consider all infectious cases, raise 
		// exception if no existing case
		if (agent.exposed() == true){
			contributions.compute_exposed_contributions(agent, time, households, 
							schools, workplaces);
		}else if (agent.symptomatic() == true){
			contributions.compute_symptomatic_contributions(agent, time, households, 
							schools, workplaces);
		}else{
			throw std::runtime_error("Agent does not have any state");
		}
	}
	contributions.total_place_contributions(households, schools, 
											workplaces);
}

// Determine infection propagation and
// state changes 
void ABM::compute_state_transitions()
{
	int newly_infected = 0, is_recovered = 0;
	// First entry is one if agent recovered, second if agent died
	std::vector<int> removed = {0,0};

	for (auto& agent : agents){

		// Skip the removed 
		if (agent.removed() == true){
			continue;
		}

		if (agent.infected() == false){
			newly_infected = transitions.susceptible_transitions(agent, time,
							dt, infection, households, schools, workplaces, infection_parameters, agents);
			n_infected_tot += newly_infected;
		}else if (agent.exposed() == true){
			is_recovered = transitions.exposed_transitions(agent, infection, time, dt, 
										households, schools, workplaces, infection_parameters);
			n_recovered_tot += is_recovered;
		}else if (agent.symptomatic() == true){
			removed = transitions.symptomatic_transitions(agent, time, dt,
							infection, households, schools, workplaces, infection_parameters);
			n_recovered_tot += removed.at(0);
			n_dead_tot += removed.at(1);
		}else{
			throw std::runtime_error("Agent does not have any infection-related state");
		}		
	}
}

//
// Getters
//
int ABM::get_num_susceptible() const {
    int susceptible_count = 0;
    for (const auto& agent : agents){
        if (!agent.infected() && !agent.exposed() && !agent.get_dead() && !agent.get_recovered())
            ++susceptible_count;
    }
    return susceptible_count;
}

/// Retrieve number of infected agents at this time step
int ABM::get_num_infected() const
{
	int infected_count = 0;
	for (const auto& agent : agents){
		if (agent.infected())
			++infected_count;
	}
	return infected_count;
}

/// Retrieve number of exposed agents at this time step
int ABM::get_num_exposed() const
{
	int exposed_count = 0;
	for (const auto& agent : agents){
		if (agent.exposed())
			++exposed_count;
	}
	return exposed_count;
}

int ABM::get_num_removed() const
{
    int removed_count = 0;
    for (const auto& agent : agents){
        if (agent.removed())
            ++removed_count;
    }
    return removed_count;
}

//
// I/O
//

// General function for reading an object from a file
std::vector<std::vector<std::string>> ABM::read_object(std::string fname)
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};

	// 2D vector with a parsed line in each inner
	// vector
	std::vector<std::vector<std::string>> obj_vec_2D;

	// Read and return a copy
	AbmIO abm_io(fname, delim, sflag, dims);
	obj_vec_2D = abm_io.read_vector<std::string>();

	return obj_vec_2D;
}

//
// Saving simulation state
//

// Save infection information
void ABM::print_infection_parameters(const std::string filename) const
{
	FileHandler file(filename, std::ios_base::out | std::ios_base::trunc);
	std::fstream &out = file.get_stream();	

	for (const auto& entry : infection_parameters){
		out << entry.first << " " << entry.second << "\n";
	}	
}

// Save age-dependent distributions
void ABM::print_age_dependent_distributions(const std::string filename) const
{
	FileHandler file(filename, std::ios_base::out | std::ios_base::trunc);
	std::fstream &out = file.get_stream();	

	for (const auto& entry : age_dependent_distributions){
		out << entry.first << "\n";
		for (const auto& e : entry.second)
			out << e.first << " " << e.second << "\n";
	}	
}


// Save current household information to file 
void ABM::print_households(const std::string fname) const
{
	print_places<Household>(households, fname);
}	

// Save current school information to file 
void ABM::print_schools(const std::string fname) const
{
	print_places<School>(schools, fname);
}


// Save current workplaces information to file 
void ABM::print_workplaces(const std::string fname) const
{
	print_places<Workplace>(workplaces, fname);
}

// Save IDs of all agents in all households
void ABM::print_agents_in_households(const std::string filename) const
{
	print_agents_in_places<Household>(households, filename);
}

// Save IDs of all agents in all schools
void ABM::print_agents_in_schools(const std::string filename) const
{
	print_agents_in_places<School>(schools, filename);
}

// Save IDs of all agents in all workplaces 
void ABM::print_agents_in_workplaces(const std::string filename) const
{
	print_agents_in_places<Workplace>(workplaces, filename);
}


// Save current agent information to file 
void ABM::print_agents(const std::string fname) const
{
	// AbmIO settings
	std::string delim(" ");
	bool sflag = true;
	std::vector<size_t> dims = {0,0,0};	

	// Write data to file
	AbmIO abm_io(fname, delim, sflag, dims);
	abm_io.write_vector<Agent>(agents);	
}

void ABM::collect_all_interactions(){
    for (Agent& agent : agents){
        // First index: # of interactions Second index: # of dead
        std::vector<int> stats = agent.collect_interactions(agents);
        std::vector<int>& interactions = agent.get_all_interactions();
        interactions.push_back(stats.at(0));
    }
}

void ABM::collect_dead_interactions() {
    for (Agent& agent : agents){
        // First index: # of interactions Second index: # of dead
        std::vector<int> stats = agent.collect_interactions(agents);
        std::vector<int>& dead_interactions = agent.get_dead_interactions();
        dead_interactions.push_back(stats.at(1));
    }
}

void ABM::output_interactions(std::string filename) {
    std::ofstream out(filename);
    for (Agent& agent : agents){
        std::vector<int>& interactions = agent.get_all_interactions();
        out << agent.get_ID() << " ";
        std::copy(interactions.begin(), interactions.end(), std::ostream_iterator<int>(out," "));
        out << '\n';
    }
}

void ABM::output_dead_interactions(std::string filename) {
    std::ofstream out(filename);
    for (Agent& agent : agents){
        std::vector<int>& dead_interactions = agent.get_dead_interactions();
        out << agent.get_ID() << " ";
        std::copy(dead_interactions.begin(), dead_interactions.end(), std::ostream_iterator<int>(out," "));
        out << '\n';
    }
}



