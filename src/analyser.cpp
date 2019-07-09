/*
 * analyser.cpp
 *
 *  Created on: 07 May 2019
 *      Author: kiwoznia
 */

#include <string>
#include <iostream>
#include <utility>
#include "interface/analyser.h"
#include "interface/RechitConverter.h"
#include "interface/SimclusterConverter.h"
#include "interface/WindowEtaPhi.h"


void analyser::registerOutputVectors( TTree * tree ){

	tree->Branch("rechit_features", &_out_rechit);
	tree->Branch("rechit_simcluster_indices", &_out_rechit_simcluster_indices);
	tree->Branch("rechit_simcluster_fractions", &_out_rechit_simcluster_frac);
	tree->Branch("simcluster_statistics", &_out_simcluster_stats);
}


void analyser::clearOutputVectors( ){

	_out_rechit.clear();
	_out_rechit_simcluster_indices.clear();
	_out_rechit_simcluster_frac.clear();
	_out_simcluster_stats.clear();
}


void analyser::analyze(size_t childid /* this info can be used for printouts */){

	std::cout << "+++ reading input file " << getSampleFile() << " +++" << std::endl;
	
	/* ==SKIM==
	 *
	 * If a skim of the Delphes outout was created in a way indicated
	 * further below, use the tBranchHandler (please notice the "t")
	 * to access vectors of objects...
	 *
	 */
	 // rechit data
	 d_ana::tBranchHandler<std::vector<float> > rechit_energy( tree(),"rechit_energy");
	 d_ana::tBranchHandler<std::vector<float> > rechit_eta( tree(),"rechit_eta");
	 d_ana::tBranchHandler<std::vector<float> > rechit_phi( tree(),"rechit_phi");
	 d_ana::tBranchHandler<std::vector<float> > rechit_x( tree(),"rechit_x");
	 d_ana::tBranchHandler<std::vector<float> > rechit_y( tree(),"rechit_y");
	 d_ana::tBranchHandler<std::vector<float> > rechit_z( tree(),"rechit_z");
	 d_ana::tBranchHandler<std::vector<int> > rechit_detid( tree(),"rechit_detid");
	 d_ana::tBranchHandler<std::vector<float>> rechit_time( tree(), "rechit_time");
	 // simcluster data
	 d_ana::tBranchHandler<std::vector <std::vector<int>> > in_simcluster_hits_idx( tree(), "simcluster_hits_indices");
	 d_ana::tBranchHandler<std::vector <std::vector<float>> > in_simcluster_frac( tree(), "simcluster_fractions");
	 d_ana::tBranchHandler<std::vector<float> > in_simcluster_eta( tree(),"simcluster_eta");
	 d_ana::tBranchHandler<std::vector<float> > in_simcluster_phi( tree(),"simcluster_phi");


	/*
	 * If (optionally) a skim or a flat ntuple is to be created, please use the following function to initialize
	 * the tree. The output files will be written automatically, and a config file will be created.
	 */
	TTree* myskim=addTree();
	registerOutputVectors( myskim );

	size_t nevents=tree()->entries();
	//if(isTestMode()) nevents/=100;

	for(size_t eventno=0;eventno<nevents;eventno++){

		clearOutputVectors( );

		/* The following two lines report the status and set the event link
		 * Do not remove! */
		reportStatus(eventno,nevents);
		tree()->setEntry(eventno); // all data (energy, eta, ...) of all registered in vectors for the event read in here

		// read in rechit features and simcluster features sorted by eta
		RechitConverter rechitConv = RechitConverter( rechit_energy.content(), rechit_x.content(), rechit_y.content(), rechit_z.content(), rechit_detid.content(), rechit_phi.content(), rechit_eta.content(), rechit_time.content() );
		SimclusterConverter simclusConv = SimclusterConverter( in_simcluster_eta.content(), in_simcluster_phi.content(), in_simcluster_hits_idx.content(), in_simcluster_frac.content() );

		// get hits in window
		std::vector<int> hit_idx_in_simclusters = simclusConv.getHitIndicesBelongingToClusters();
		double window_margin = 0.05;
		WindowEtaPhi winEtaPhi( &hit_idx_in_simclusters, window_margin, rechitConv.eta(), rechitConv.phi() );
		std::vector<int> hit_indices_in_eta_phi_window = winEtaPhi.getHitIndicesInEtaPhiWindow( &hit_idx_in_simclusters, rechitConv.eta(), rechitConv.phi() ); 

		for( int hit_idx : hit_indices_in_eta_phi_window ){ // for each rechit (get number of rechits from energy feature)

			// put together all features for rechit "hit_idx"
			std::vector<float> rechit_features = rechitConv.getFeaturesForHit( hit_idx );
			_out_rechit.push_back(rechit_features);

			// get simcluster indices and their respective fractions for rechit "hit_idx"
			std::pair<std::vector<int>, std::vector<float>> simcluster_indices_and_fractions = simclusConv.getClusterIdxAndFracForHit( hit_idx ); 
			_out_rechit_simcluster_indices.push_back( simcluster_indices_and_fractions.first );
			_out_rechit_simcluster_frac.push_back( simcluster_indices_and_fractions.second );

		}

		_out_simcluster_stats = simclusConv.getStatsForSimclusters();

		myskim->Fill();

	}

	/*
	 * Must be called in the end, takes care of thread-safe writeout and
	 * call-back to the parent process
	 */
	processEndFunction();
}



void analyser::postProcess(){ }



