/*
 * GameMaster.cc
 *
 *  Created on: June 7, 2014
 *      Author: Andreea
 */

#include <iostream>
#include "GameMaster.h"
#include "../Globals.h" //TODO: find better way for this include
//#include <algorithm> //std::transform

using std::deque;
using std::map;
//using std::unordered_map;
using std::multimap;
using std::pair;
using std::cout;
using std::endl;

//TODO
void PrintGenerationInfo(const deque<Brain> brains) {
	//std::cout << "Print some info\n";
	std::cout << "Printing brain fitness scores: \n";
	for (auto brain_it = brains.begin(); brain_it != brains.end(); brain_it++) {
		std::cout << brain_it->fitness_score_ << std::endl;
	}
}

GameMaster::GameMaster(const size_t num_brains,
					   const size_t num_neurons, const size_t num_input_neurons, const size_t num_output_neurons,
					   const int max_decisions,
					   const int input_duration, const int input_output_delay, const int output_duration,
					   const string maze_map_file, const int maze_random_start,
					   const int num_generations, const size_t num_mutated_neurons, const size_t num_mutated_synapses,
					   const float prob_asexual) {

	//construct brains
	num_brains_ = num_brains;
	for (size_t ii = 0; ii < num_brains_; ii++) {
		Brain new_brain(num_neurons, num_input_neurons, num_output_neurons);
		brains_.push_back(new_brain);
	}

	//member variables for brain cycles
	max_decisions_ = max_decisions;
	input_duration_ = input_duration;
	input_output_delay_ = input_output_delay;
	output_duration_ = output_duration;

	//member variables for maze task
	string maze_map_file_ = maze_map_file;
	maze_random_start_ = maze_random_start;

	//construct evolution
	num_generations_ = num_generations;
	num_mutated_neurons_ = num_mutated_neurons;
	num_mutated_synapses_ = num_mutated_synapses;
	Evolution evolution_(prob_asexual);
}

GameMaster::GameMaster(const size_t num_brains,
					   const size_t num_neurons, const size_t num_input_neurons, const size_t num_output_neurons,
					   const float av_active_threshold, const float st_dev_active_threshold,
					   const float av_start_activation, const float st_dev_start_activation,
					   const float av_decay_rate, const float st_dev_decay_rate,
					   const int av_num_syn, const int st_dev_num_syn,
					   const float av_syn_strength, const float st_dev_syn_strength,
					   const int max_decisions,
					   const int input_duration, const int input_output_delay, const int output_duration,
					   const string maze_map_file, const int maze_random_start,
					   const int num_generations, const size_t num_mutated_neurons, const size_t num_mutated_synapses,
					   const float prob_asexual) {
	//construct brains
	num_brains_ = num_brains;
	for (size_t ii = 0; ii < num_brains_; ii++) {
		Brain new_brain(num_neurons, num_input_neurons, num_output_neurons,
						av_active_threshold, st_dev_active_threshold,
						av_start_activation, st_dev_start_activation,
						av_decay_rate, st_dev_decay_rate,
						av_num_syn, st_dev_num_syn,
						av_syn_strength, st_dev_syn_strength);
		brains_.push_back(new_brain);
	}

	//member variables for brain cycles
	max_decisions_ = max_decisions;
	input_duration_ = input_duration;
	input_output_delay_ = input_output_delay;
	output_duration_ = output_duration;

	//member variables for maze task
	maze_map_file_ = maze_map_file;
	maze_random_start_ = maze_random_start;

	//construct evolution
	num_generations_ = num_generations;
	num_mutated_neurons_ = num_mutated_neurons;
	num_mutated_synapses_ = num_mutated_synapses;
	Evolution evolution_(prob_asexual);
}

void GameMaster::ObtainBrainFitnesses() {
	cout << "Entered ObtainBrainFitnesses\n";
	cout << brains_.size() << endl;
	for (auto brain_it = brains_.begin(); brain_it != brains_.end(); brain_it++) {
	   //Default to worst outcome. If the brain doesn't finish the maze in max_decisions_ time then it receives the worst score.
		int num_decisions = max_decisions_;

        MazeTask maze_task(maze_map_file_, maze_random_start_);

		//Loop through every decision the brain must make
		for (int decision = 0; decision < max_decisions_; decision++) {
			//Advance position through the maze (either from the starting line or from the previous decision)
            if (!maze_task.AdvancePosition()) {
				//Brain tried to enter a wall; receive worst fitness score
                break;
            }

            //Check if brain has finished the maze
            if (maze_task.IsFinished()) {
                num_decisions = decision;
                break;
            }

			//Get brain input (which are the possible directions from this position)
            deque<bool> brain_input = maze_task.GetBrainInput();

            //Let the brain decide on an action
			//The first element in the map is an output of the brain, while the second is counting how many times
                //the brain has given this output
			//TODO: this doesn't need to be an ordered map, but if I try to make it unordered_map I get errors later. Don't fully understand why.
			map<deque<bool>, int> brain_output_frequency;
            for (int cycle = 0; cycle < (input_output_delay_ + output_duration_); cycle++) {
                if (cycle < input_duration_) {
					brain_it->give_input(brain_input); //give input to brain at each cycle
                }
                brain_it->Cycle();
				if (cycle >= input_output_delay_) {
					deque<bool> brain_output = brain_it->get_output();	//output given at the current cycle
					if (brain_output_frequency.count(brain_output)) { //this brain_output is already in the map
                        brain_output_frequency[brain_output]++; //increase the frequency
					} else { //this output appears for the first time
                        brain_output_frequency[brain_output] = 1;
                    }
                }
            }

			//Test the brain_outputs in order of the frequency, and try to find the first (namely most common) valid output (that doesn't send the brain into a wall)
			//flip the map of <brain_output, frequency> to a multimap <frequency, brain_output> (which is now sorted by its frequency
			multimap<int, deque<bool>> brain_output_sorted;
			for (auto map_it = brain_output_frequency.begin(); map_it != brain_output_frequency.end(); map_it++) {
				brain_output_sorted.insert( pair<int, deque<bool> >(map_it->second, map_it->first) );
			}
			bool found_valid_decision = false;
			deque<bool> brain_decision; //final (most common and valid) brain output will correspond to the decision
            for (auto output_it = brain_output_sorted.begin(); output_it != brain_output_sorted.end(); output_it++) {
				if (maze_task.ActOnDecision(output_it->second)) { //succes! the brain has found a valid decision, and the brain has turned in the new direction
                    found_valid_decision = true;
					brain_decision = output_it->second;
                    break;
                }
            }

			//If no valid decision has been found, the brain decided to go into a wall, so it gets the worst fitness score
            if (!found_valid_decision) {
                break;
            }

		} //end for loop through decisions


        //Set fitness score to 1/num_decisions
		brain_it->set_fitness_score(1.0/num_decisions);
		cout << brain_it->get_fitness_score() << endl;
		cout << "Here1" << endl;
	} //end for loop through every brain
	cout << "Finished ObtainBrainFitnesses" << endl;
}


int GameMaster::MasterControl() {
	for (int gener = 0; gener < num_generations_; gener++) {
		std::cout << "Generation " << gener << std::endl;
		//Obtain the fitness scores for each brain
		ObtainBrainFitnesses();
		std::cout << "Exited ObtainBrainFitnesses" << endl;

		//find the list of most fit brains
		int test = evolution_.ChooseMostFitBrains(brains_);
		if (test < 0) {
			return -1;
		}

		//Obtain the next generation of brains
		brains_ = evolution_.GetNextGeneration(brains_, num_mutated_neurons_, num_mutated_synapses_);

		PrintGenerationInfo(brains_);
	}

	return 0;
}



