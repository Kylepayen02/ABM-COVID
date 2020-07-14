#include "../../../include/abm.h"
#include <chrono>

/***************************************************** 
 *
 * Simple ABM run 
 *
 ******************************************************/

int main()
{
	// Time in days, space in km
	double dt = 0.25;
	// Max number of days to simulate
	int tmax = 400;	

	// Infection parameters
	// Within-household transmission rate 
	// 0.09, 0.9, 9.0
	double beta_h = 0.9;
	// Within-school transmission rate
	// 0.05, 0.5, 5.0
	double beta_s = 0.5;
	// Within-workplace transmission rate
	// 0.03, 0.3, 3.0
	double beta_w = 0.3;
	// Transmission rate in general community
	// 0.01, 0.1. 1.0
	double beta_u = 0.1;
	// Parameters for scaling with distance
	// a has units of distance
	// 0.38, 3.8, 38.0
	// New a: 0.0038, 0.038, 0.38 
	double a = 3.8;
	double b = 2.32;
	// Parameter for scaling of household transmission rates 
	// with household size
	double alpha = 0.8;
	// Average infectious period
	// 1.0, 10.0, 100.0
	double rec = 10.0; 
		
	// Input files
	std::string fin("input_data/agents_data.txt");
	std::string hfile("input_data/houses_data.txt");
	std::string sfile("input_data/schools_data.txt");
	std::string wfile("input_data/workplaces_data.txt");

	ABM abm(dt, beta_h, beta_s, beta_w, beta_u, a, b, alpha, rec);

	// First the places
	abm.create_households(hfile);
	abm.create_schools(sfile);
	abm.create_workplaces(wfile);

	// Then the agents
	abm.create_agents(fin);
	
	// Simulation
	// Collect infected agents and save
	std::vector<int> infected_count(tmax+1);

	// For time measurement
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (int ti = 0; ti<=tmax; ++ti){
		infected_count.at(ti) = abm.get_num_infected();	
		abm.transmit_infection();
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds> (end - begin).count() << "[s]" << std::endl;

	// Output innfected
	std::ofstream out("parametric_results.txt");
	std::copy(infected_count.begin(), infected_count.end(), std::ostream_iterator<int>(out," "));
}
