#include "../../include/abm.h"
#include <chrono>

/***************************************************** 
 *
 * ABM run of COVID-19 SEIR in New Rochelle, NY 
 *
 ******************************************************/

int main()
{
	// Time in days, space in km
	double dt = 0.25;
	// Max number of steps to simulate
	int tmax = 400;	
	// Print agent info this many steps
	int dt_out_agents = 401;

	// Input files
	std::string fin("input_data/NR_agents.txt");
	std::string hfile("input_data/NR_households.txt");
	std::string sfile("input_data/NR_schools.txt");
	std::string wfile("input_data/NR_workplaces.txt");


	// File with infection parameters
	std::string pfname("input_data/infection_parameters.txt");
	// Files with age-dependent distributions;
	std::string dmort_name("input_data/age_dist_mortality.txt");
	// Map for abm loading of distributions
	std::map<std::string, std::string> dfiles = 
		{ {"mortality", dmort_name} };

	ABM abm(dt, pfname, dfiles);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);

	// Then the agents
	abm.create_agents(fin);

	std::vector<Agent>* agents = &(abm.get_vector_of_agents_non_const());
	
	// Simulation
	// Collect infected agents and save
	std::vector<int> infected_count(tmax+1);
	// Add exposed too

	// For time measurement
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (int ti = 0; ti<=tmax; ++ti){
		// Save agent information
/*		if (ti%dt_out_agents == 0){
			std::string fname = "output/agents_t_" + std::to_string(ti) + ".txt";
			abm.print_agents(fname);
		}*/

        //Get the number of interactions for each agent
        if (ti == 0){
            abm.collect_all_interactions();
        }

		infected_count.at(ti) = abm.get_num_infected();	
		abm.transmit_infection();
		abm.collect_dead_interactions();
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;

	// Output infected
	std::ofstream out("output/infected_with_time.txt");
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(out," "));

	// Output interactions
    abm.output_interactions("interactions.txt");

    //Output dead interactions
    abm.output_dead_interactions("dead_interactions.txt");

	// Print total values
	std::cout << "Total number of infected agents: " << abm.get_total_infected() << "\n"
			  << "Total number of casualities: " << abm.get_total_dead() << "\n"
			  << "Total number of recovered agents: " << abm.get_total_recovered() << "\n";
}