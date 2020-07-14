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
	int dt_out_agents = 100; 

	// Input files
	std::string fin("input_data/NR_agents.txt");
	std::string hfile("input_data/NR_households.txt");
	std::string sfile("input_data/NR_schools.txt");
	std::string wfile("input_data/NR_workplaces.txt");
	std::string hsp_file("input_data/NR_hospitals.txt");

	// File with infection parameters
	std::string pfname("input_data/infection_parameters.txt");
	// Files with age-dependent distributions
	std::string dh_name("input_data/age_dist_hospitalization.txt");
	std::string dhicu_name("input_data/age_dist_hosp_ICU.txt");
	std::string dmort_name("input_data/age_dist_mortality.txt");
	// Map for abm loading of distrinutions
	std::map<std::string, std::string> dfiles = 
		{ {"hospitalization", dh_name}, {"ICU", dhicu_name}, {"mortality", dmort_name} };	

	ABM abm(dt, pfname, dfiles);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);
	abm.create_hospitals(hsp_file);

	// Then the agents
	abm.create_agents(fin);
	
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
		infected_count.at(ti) = abm.get_num_infected();	
		abm.transmit_infection();
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;

	// Output infected
	std::ofstream out("output/infected_with_time.txt");
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(out," "));

	// Print total values
	std::cout << "Total number of infected agents: " << abm.get_total_infected() << "\n"
			  << "Total number of casualities: " << abm.get_total_dead() << "\n"
			  << "Total number of recovered agents: " << abm.get_total_recovered() << "\n";
}
