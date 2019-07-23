/*
 * caloParticleMerger.cpp
 *
 *  Created on: 22 Jul 2019
 *      Author: jkiesele
 */

#include "interface/caloParticleMerger.h"
#include <string> //dirty hack to get size_t
#include <algorithm>
#include <iostream>

void mergeSimclusters(std::vector<int>& mergewithidxs, std::vector<float>& mergewithfracs,
         const std::vector<int>& tomergeidxs,  const std::vector<float>& tomergefracs
        ){
    //simple loop for now
    for(size_t mi=0;mi<tomergeidxs.size();mi++){
        auto foundat = std::find(mergewithidxs.begin(),mergewithidxs.end(),tomergeidxs.at(mi));
        if(foundat == mergewithidxs.end()){
            mergewithidxs.push_back(tomergeidxs.at(mi));
            mergewithfracs.push_back(tomergefracs.at(mi));
        }
        else{
            size_t idx = foundat - mergewithidxs.begin();
            mergewithfracs.at(idx) += tomergefracs.at(mi);
        }

    }


}

caloParticleMerger::caloParticleMerger(std::vector<std::vector<int> > * calopart_simClusterIndex,
            std::vector<std::vector<float> > * sc_frac,
            std::vector<std::vector<int> > * hits_idx){

    //calopart_simClusterIndex: calopart, SC

    int total_hits_merged=0;

    for(size_t i=0;i<calopart_simClusterIndex->size();i++){
        std::vector<int> this_hits;
        std::vector<float> this_fracs;
        for(size_t j=0;j<calopart_simClusterIndex->at(i).size();j++){

            mergeSimclusters(this_hits, this_fracs,
                    hits_idx->at(calopart_simClusterIndex->at(i).at(j)),
                    sc_frac->at(calopart_simClusterIndex->at(i).at(j)));

        }
        total_hits_merged+=this_hits.size();
        _merged_hitidx.push_back(this_hits);
        _merged_fractions.push_back(this_fracs);
    }

}
