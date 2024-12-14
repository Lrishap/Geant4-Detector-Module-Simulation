//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file optical/wls/src/WLSStackingAction.cc
/// \brief Implementation of the WLSStackingAction class
//
//
#include "WLSStackingAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"


#include <iostream>
#include <fstream> // Required for file handling

#include "G4AnalysisManager.hh" // Ntuples update
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSStackingAction::WLSStackingAction()
  : fPhotonCounter(0)
  , fScintillationCounter(0)
  , fCerenkovCounter(0)

{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSStackingAction::~WLSStackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack WLSStackingAction::ClassifyNewTrack(
  const G4Track* aTrack)
{
  G4ParticleDefinition* particleType = aTrack->GetDefinition();

  // keep primary particle
  if (aTrack->GetParentID() == 0) { return fUrgent; }
  
  if(particleType == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    // keep optical photon
    ++fPhotonCounter;
    return fUrgent;
  }


 //stack and count optical photons 
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {  // particle is optical photon
 //   ++fPhotonCounter; // total optical photons produced
    if(aTrack->GetParentID() > 0)
    {  // particle is secondary
      if(aTrack->GetCreatorProcess()->GetProcessName() == "Scintillation")
        ++fScintillationCounter;
      else if(aTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")
        ++fCerenkovCounter;
    }
  }


  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSStackingAction::NewStage()
{
  // G4cout << "Number of optical photons produces in this event : "
  //        << fPhotonCounter << G4endl;

// Open a file stream for writing
std::ofstream outputFile("scintillation_output.txt", std::ios::app);

// Check if the file is successfully opened
if (outputFile.is_open()) {
    // Redirect the output to the file stream
    outputFile << fPhotonCounter << std::endl;

    // Close the file stream
    outputFile.close();
} else {
    // Print an error message if the file cannot be opened
    G4cout << "Error: Unable to open the output file." << G4endl;
}
  
 auto analysisManager = G4AnalysisManager::Instance();
 analysisManager->FillNtupleDColumn(1,1,fPhotonCounter);
 //analysisManager->AddNtupleRow();

         
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSStackingAction::PrepareNewEvent() { 
  fPhotonCounter = 0; 
  fScintillationCounter = 0; 
  fCerenkovCounter =0;
  }
