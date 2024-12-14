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
/// \file optical/wls/src/WLSRunAction.cc
/// \brief Implementation of the WLSRunAction class
//
//

#include "WLSRunAction.hh"

#include "WLSDetectorConstruction.hh"
#include "WLSRun.hh"
#include "WLSSteppingAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSRunAction::WLSRunAction()
  : fRun(nullptr)
{
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("root");
  analysisManager->SetVerboseLevel(1);
  G4cout << "Using " << analysisManager->GetType() << G4endl;

	// Default settings
  analysisManager->SetNtupleMerging(true);
  // Note: merging ntuples is available only with Root output
  analysisManager->SetFileName("MAPP_2");

  // Creating ntuple

  analysisManager->CreateNtuple("WLS", "WLS");
  analysisManager->CreateNtupleDColumn("Energy");
  analysisManager->CreateNtupleDColumn("DetectorID");
  analysisManager->CreateNtupleDColumn("EventID");
  analysisManager->FinishNtuple();
	analysisManager->SetNtupleFileName(0,"WLS_energy");


  analysisManager->CreateNtuple("Hits", "Hits and Optical Photons generated");
  analysisManager->CreateNtupleDColumn("Hits");
  analysisManager->CreateNtupleDColumn("OpticalPhotons");
  analysisManager->CreateNtupleDColumn("NumberofEvents");
  analysisManager->FinishNtuple();
	analysisManager->SetNtupleFileName(1,"WLS_hits");



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSRunAction::~WLSRunAction() {}

G4Run* WLSRunAction::GenerateRun()
{
  fRun = new WLSRun();
  return fRun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSRunAction::BeginOfRunAction(const G4Run*)
{
 // G4AnalysisManager::Instance()->OpenFile("wls");

	// Get analysis manager
  	auto analysisManager = G4AnalysisManager::Instance();
  	// Open an output file
  	// The default file name is set in RunAction::RunAction(),
  	// it can be overwritten in a macro
  	analysisManager->OpenFile();



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSRunAction::EndOfRunAction(const G4Run*)
{
  auto analysisManager = G4AnalysisManager::Instance();
  // if (analysisManager->GetH1(0)) {
  //   G4cout << G4endl << " ----> print histograms statistics ";
  //   if(isMaster)
	// 	{
  //     G4cout << "for the entire run " << G4endl << G4endl;
  //   }
  //   else {
  //     G4cout << "for the local thread " << G4endl << G4endl;
  //   }

  //   G4cout << " Mean number of photons detected/event: "
  //      << analysisManager->GetH1(2)->mean()
  //      << " rms = "
  //      << analysisManager->GetH1(2)->rms() << G4endl;

  // }

  analysisManager->Write();
  analysisManager->CloseFile();


  if(isMaster)
    fRun->EndOfRun();
}