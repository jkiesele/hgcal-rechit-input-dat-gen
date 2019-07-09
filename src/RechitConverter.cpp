#include "interface/RechitConverter.h"

#include <algorithm>
#include <numeric>
#include <iostream>
#include "Math/Vector3D.h"


RechitConverter::RechitConverter( std::vector<float> * en, std::vector<float> * xx, std::vector<float> * yy, std::vector<float> * zz, std::vector<int> * dd, std::vector<float> * pp, std::vector<float> * et, std::vector<float> * tt ):
                                            _energy( en ), _x( xx ), _y( yy ), _z( zz ), _detid( dd ), _phi( pp ), _eta( et ), _time( tt ) { 

                                                //std::transform( _eta->begin(), _eta->end(), std::back_inserter( _theta ), computeTheta ); // compute theta
                                            }

std::vector<float> RechitConverter::getFeaturesForHit( int rechit_idx ){

    ROOT::Math::DisplacementVector3D<ROOT::Math::CylindricalEta3D<double> > RhoEtaPhiVector;
    RhoEtaPhiVector.SetXYZ(_x->at(rechit_idx), _y->at(rechit_idx),_z->at(rechit_idx));

    return std::vector<float>{ _energy->at(rechit_idx), _eta->at(rechit_idx), _phi->at(rechit_idx), static_cast<float>(RhoEtaPhiVector.Theta()), static_cast<float>(RhoEtaPhiVector.R()), 
                                _x->at(rechit_idx), _y->at(rechit_idx), _z->at(rechit_idx), static_cast<float>(_detid->at(rechit_idx)), _time->at( rechit_idx ) };
}