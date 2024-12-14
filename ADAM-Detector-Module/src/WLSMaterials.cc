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
/// \file optical/wls/src/WLSMaterials.cc
/// \brief Implementation of the WLSMaterials class
//
//
#include "WLSMaterials.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"


#include "G4Material.hh"

WLSMaterials* WLSMaterials::fInstance = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSMaterials::WLSMaterials()
{
  fNistMan = G4NistManager::Instance();
  fNistMan->SetVerbose(2);

  CreateMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSMaterials::~WLSMaterials()
{
  delete fPMMA;
  delete fPethylene;
  delete fFPethylene;
  delete fPolystyrene;
  delete fSilicone;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSMaterials* WLSMaterials::GetInstance()
{
  if(!fInstance)
  {
    fInstance = new WLSMaterials();
  }
  return fInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* WLSMaterials::GetMaterial(const G4String material)
{
  G4Material* mat = fNistMan->FindOrBuildMaterial(material);

  if(!mat)
    mat = G4Material::GetMaterial(material);
  if(!mat)
  {
    G4ExceptionDescription ed;
    ed << "Material " << material << " not found!";
    G4Exception("WLSMaterials::GetMaterial", "", FatalException, ed);
  }

  return mat;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSMaterials::CreateMaterials()
{
  G4double density;
  G4int ncomponents;
  G4double fractionmass;
  std::vector<G4int> natoms;
  std::vector<G4double> fractionMass;
  std::vector<G4String> elements;

  // Materials Definitions
  // =====================

  //--------------------------------------------------
  // Vacuum
  //--------------------------------------------------

  fNistMan->FindOrBuildMaterial("G4_Galactic");

  //--------------------------------------------------
  // Air
  //--------------------------------------------------

  fAir = fNistMan->FindOrBuildMaterial("G4_AIR");

  //--------------------------------------------------
  // Cladding (polyethylene)
  //--------------------------------------------------

  elements.push_back("C");
  natoms.push_back(2);
  elements.push_back("H");
  natoms.push_back(4);

  density = 1.200 * g / cm3;

  fPethylene =
    fNistMan->ConstructNewMaterial("Pethylene", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------
  // Double Cladding (fluorinated polyethylene)
  //--------------------------------------------------

  elements.push_back("C");
  natoms.push_back(2);
  elements.push_back("H");
  natoms.push_back(4);

  density = 1.430 * g / cm3;

  fFPethylene =
    fNistMan->ConstructNewMaterial("FPethylene", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------
  // Polystyrene
  //--------------------------------------------------

  elements.push_back("C");
  natoms.push_back(8);
  elements.push_back("H");
  natoms.push_back(8);

  density = 1.050 * g / cm3;

  fPolystyrene =
    fNistMan->ConstructNewMaterial("Polystyrene", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //-------------------------------------------------------------------------------------------------
  // WLS Polystyrene [for core as per https://www.kuraray.com/uploads/5a717515df6f5/PR0150_psf01.pdf]
  //-------------------------------------------------------------------------------------------------

  elements.push_back("C");
  natoms.push_back(8);
  elements.push_back("H");
  natoms.push_back(8);

  density = 1.050 * g / cm3;

  fWLSPolystyrene = fNistMan->ConstructNewMaterial("WLSPolystyrene", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------------------------------------------------------

  //--------------------------------------------------
  // Silicone (Template for Optical Grease)
  //--------------------------------------------------

  elements.push_back("C");
  natoms.push_back(2);
  elements.push_back("H");
  natoms.push_back(6);

  density = 1.060 * g / cm3;

  fSilicone = fNistMan->ConstructNewMaterial("Silicone", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------
  // Aluminium
  //--------------------------------------------------

  fNistMan->FindOrBuildMaterial("G4_Al");

  //--------------------------------------------------
  // TiO2
  //--------------------------------------------------

  elements.push_back("Ti");
  natoms.push_back(1);
  elements.push_back("O");
  natoms.push_back(2);

  density = 4.26 * g / cm3;

  G4Material* TiO2 =
    fNistMan->ConstructNewMaterial("TiO2", elements, natoms, density);

  elements.clear();
  natoms.clear();

  //--------------------------------------------------
  // Scintillator Coating - 15% TiO2 and 85% polystyrene by weight.
  //--------------------------------------------------

  density = 1.52 * g / cm3;

  fCoating = new G4Material("Coating", density, ncomponents = 2);

  fCoating->AddMaterial(TiO2, fractionmass = 15 * perCent);
  fCoating->AddMaterial(fPolystyrene, fractionmass = 85 * perCent);

  //
  // ------------ Generate & Add Material Properties Table ------------
  //

  
// 64 entries of BC404 
 std::vector<G4double> BC404_energy = {
    2.382 * eV, 2.411 * eV, 2.438 * eV, 2.467 * eV, 2.493 * eV, 2.517 * eV, 2.543 * eV, 2.565 * eV, 2.587 * eV, 2.607 * eV,
    2.625 * eV, 2.642 * eV, 2.657 * eV, 2.667 * eV, 2.678 * eV, 2.688 * eV, 2.706 * eV, 2.721 * eV, 2.742 * eV, 2.749 * eV,
    2.76 * eV, 2.776 * eV, 2.791 * eV, 2.804 * eV, 2.805 * eV, 2.814 * eV, 2.822 * eV, 2.828 * eV, 2.831 * eV, 2.832 * eV,
    2.841 * eV, 2.85 * eV, 2.857 * eV, 2.879 * eV, 2.898 * eV, 2.912 * eV, 2.924 * eV, 2.933 * eV, 2.947 * eV, 2.962 * eV,
    2.973 * eV, 2.983 * eV, 2.989 * eV, 3.0 * eV, 3.01 * eV, 3.012 * eV, 3.014 * eV, 3.026 * eV, 3.03 * eV, 3.036 * eV,
    3.044 * eV, 3.05 * eV, 3.06 * eV, 3.071 * eV, 3.081 * eV, 3.094 * eV, 3.104 * eV, 3.12 * eV, 3.145 * eV, 3.166 * eV,
    3.205 * eV, 3.26 * eV, 3.332 * eV, 3.432 * eV
};

  //--------------------------------------------------
  // Air [Optical Property]
  //--------------------------------------------------

  std::vector<G4double> refractiveIndex(64,1.0);

  G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
  mpt->AddProperty("RINDEX", BC404_energy, refractiveIndex);

  fAir->SetMaterialPropertiesTable(mpt);

  //--------------------------------------------------
  //  PMMA for WLSfibers
  //--------------------------------------------------

// 94 entries of energyWLS
std::vector<G4double> energyWLSABS = {
    1.775 * eV, 1.783 * eV, 1.786 * eV, 1.790 * eV, 1.793 * eV, 1.799 * eV, 1.804 * eV, 1.807 * eV, 1.813 * eV, 1.818 * eV,
    1.821 * eV, 1.824 * eV, 1.825 * eV, 1.828 * eV, 1.828 * eV, 1.835 * eV, 1.843 * eV, 1.852 * eV, 1.853 * eV, 1.861 * eV,
    1.873 * eV, 1.873 * eV, 1.882 * eV, 1.889 * eV, 1.893 * eV, 1.897 * eV, 1.900 * eV, 1.906 * eV, 1.909 * eV, 1.918 * eV,
    1.933 * eV, 1.934 * eV, 1.944 * eV, 1.952 * eV, 1.962 * eV, 1.979 * eV, 1.993 * eV, 1.998 * eV, 2.001 * eV, 2.007 * eV,
    2.008 * eV, 2.017 * eV, 2.018 * eV, 2.021 * eV, 2.023 * eV, 2.027 * eV, 2.027 * eV, 2.030 * eV, 2.033 * eV, 2.043 * eV,
    2.056 * eV, 2.064 * eV, 2.079 * eV, 2.085 * eV, 2.089 * eV, 2.110 * eV, 2.133 * eV, 2.168 * eV, 2.200 * eV, 2.243 * eV,
    2.271 * eV, 2.282 * eV, 2.298 * eV, 2.315 * eV, 2.324 * eV, 2.350 * eV, 2.372 * eV, 2.409 * eV, 2.440 * eV, 2.460 * eV,
    2.472 * eV, 2.481 * eV, 2.512 * eV, 2.555 * eV, 2.567 * eV, 2.653 * eV, 2.715 * eV, 2.794 * eV, 2.911 * eV, 2.956 * eV,
    2.978 * eV, 3.000 * eV, 3.084 * eV, 3.149 * eV, 3.284 * eV, 3.399 * eV, 3.439 * eV, 3.551 * eV, 3.680 * eV, 3.691 * eV,
    3.840 * eV, 3.879 * eV, 4.023 * eV, 4.133 * eV};

// 94 entries for absorption length 

std::vector<G4double> absWLSfiber =
{
    6.907 * m, 7.860 * m, 8.918 * m, 10.050 * m, 11.241 * m, 12.242 * m, 13.166 * m, 14.411 * m, 15.487 * m, 16.552 * m,
    17.493 * m, 18.212 * m, 18.908 * m, 20.525 * m, 19.747 * m, 21.248 * m, 21.728 * m, 21.825 * m, 22.333 * m, 21.479 * m,
    20.364 * m, 20.958 * m, 19.539 * m, 18.536 * m, 17.527 * m, 16.556 * m, 15.639 * m, 14.537 * m, 13.651 * m, 12.802 * m,
    14.429 * m, 13.511 * m, 15.287 * m, 17.150 * m, 18.994 * m, 20.328 * m, 20.403 * m, 19.522 * m, 18.969 * m, 17.927 * m,
    16.819 * m, 14.313 * m, 12.974 * m, 11.903 * m, 10.842 * m, 10.086 * m, 9.044 * m, 7.403 * m, 6.493 * m, 6.095 * m,
    7.318 * m, 8.655 * m, 12.647 * m, 13.910 * m, 15.409 * m, 17.662 * m, 17.662 * m, 15.320 * m, 14.610 * m, 13.447 * m,
    12.805 * m, 11.986 * m, 10.711 * m, 9.659 * m, 8.154 * m, 8.330 * m, 7.949 * m, 6.650 * m, 4.110 * m, 2.771 * m,
    1.902 * m, 0.950 * m, 0.383 * m, 0.083 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m,
    0 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m, 0 * m,
    0 * m, 0 * m, 0 * m, 0 * m};


// Energy of emission spectrum [different from Absorption spectrum see: https://www.zeuthen.desy.de/lcdet/Feb_05_WS/talks/rd_lcdet_sim.pdf ]
std::vector<G4double> energyWLSEmission = {
    1.986 * eV, 2.086 * eV, 2.093 * eV, 2.100 * eV, 2.108 * eV, 2.115 * eV, 2.120 * eV, 2.128 * eV, 2.136 * eV, 2.144 * eV,
    2.151 * eV, 2.158 * eV, 2.165 * eV, 2.173 * eV, 2.181 * eV, 2.188 * eV, 2.196 * eV, 2.205 * eV, 2.212 * eV, 2.220 * eV,
    2.227 * eV, 2.235 * eV, 2.242 * eV, 2.251 * eV, 2.259 * eV, 2.267 * eV, 2.275 * eV, 2.283 * eV, 2.291 * eV, 2.299 * eV,
    2.308 * eV, 2.317 * eV, 2.325 * eV, 2.334 * eV, 2.342 * eV, 2.351 * eV, 2.360 * eV, 2.368 * eV, 2.377 * eV, 2.386 * eV,
    2.395 * eV, 2.404 * eV, 2.414 * eV, 2.423 * eV, 2.432 * eV, 2.442 * eV, 2.451 * eV, 2.459 * eV, 2.462 * eV, 2.465 * eV,
    2.467 * eV, 2.470 * eV, 2.473 * eV, 2.475 * eV, 2.478 * eV, 2.480 * eV, 2.483 * eV, 2.487 * eV, 2.491 * eV, 2.494 * eV,
    2.497 * eV, 2.501 * eV, 2.507 * eV, 2.515 * eV, 2.525 * eV, 2.536 * eV, 2.558 * eV};

//Corresponding Relative Intensity for the Emission spectrum
std::vector<G4double> emissionFib = {0.000, 0.037, 0.040, 0.041, 0.043, 0.046, 0.047, 0.049, 0.052, 0.054, 
0.057, 0.060, 0.064, 0.069, 0.075, 0.080, 0.086, 0.093, 0.102, 0.113, 
0.120, 0.130, 0.143, 0.159, 0.176, 0.194, 0.209, 0.222, 0.233, 0.245,
 0.256, 0.267, 0.274, 0.281, 0.287, 0.292, 0.296, 0.302, 0.308, 0.315, 
 0.322, 0.328, 0.336, 0.342, 0.344, 0.341, 0.334, 0.318, 0.299, 0.280, 
 0.260, 0.242, 0.222, 0.203, 0.184, 0.165, 0.144, 0.124, 0.105, 0.087, 
 0.069, 0.052, 0.031, 0.022, 0.015, 0.010, 0.000};




// 94 entries 
  std::vector<G4double> refractiveIndexWLSfiber(94, 1.59);


  // Add entries into properties table
  G4MaterialPropertiesTable* mptWLSfiber = new G4MaterialPropertiesTable();
  mptWLSfiber->AddProperty("RINDEX", energyWLSABS, refractiveIndexWLSfiber);
  mptWLSfiber->AddProperty("WLSABSLENGTH", energyWLSABS, absWLSfiber); //WLSABSLENGTH is the absorption length of the material as a function of the photon’s energy
  mptWLSfiber->AddProperty("WLSCOMPONENT", energyWLSEmission, emissionFib); //WLSCOMPONENT is the relative emission spectrum of the material as a function of the photon’s energy
  mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 6.9 * ns);

 // fPMMA->SetMaterialPropertiesTable(mptWLSfiber); // core is no longer PMMA --> https://www.kuraray.com/uploads/5a717515df6f5/PR0150_psf01.pdf

  fWLSPolystyrene->SetMaterialPropertiesTable(mptWLSfiber);


  //--------------------------------------------------
  //  Polyethylene
  //--------------------------------------------------

  std::vector<G4double> refractiveIndexClad1(64,1.49); 

  std::vector<G4double> absClad(64, 20.0 * m);

  // Add entries into properties table ---> match Plastic Scintillator
  G4MaterialPropertiesTable* mptClad1 = new G4MaterialPropertiesTable();
  mptClad1->AddProperty("RINDEX", BC404_energy, refractiveIndexClad1);
  mptClad1->AddProperty("ABSLENGTH", BC404_energy, absClad);

  fPethylene->SetMaterialPropertiesTable(mptClad1);

  //--------------------------------------------------
  // Fluorinated Polyethylene
  //--------------------------------------------------

  std::vector<G4double> refractiveIndexClad2(64,1.42); 

  // Add entries into properties table
  G4MaterialPropertiesTable* mptClad2 = new G4MaterialPropertiesTable();
  mptClad2->AddProperty("RINDEX", BC404_energy, refractiveIndexClad2);
  mptClad2->AddProperty("ABSLENGTH", BC404_energy, absClad);

  fFPethylene->SetMaterialPropertiesTable(mptClad2);

  //--------------------------------------------------
  // Silicone
  //--------------------------------------------------

  std::vector<G4double> refractiveIndexSilicone(64,1.46);

  // Add entries into properties table
  G4MaterialPropertiesTable* mptSilicone = new G4MaterialPropertiesTable();
  mptSilicone->AddProperty("RINDEX", BC404_energy, refractiveIndexSilicone);
  mptSilicone->AddProperty("ABSLENGTH", BC404_energy, absClad);

  fSilicone->SetMaterialPropertiesTable(mptSilicone);

  //--------------------------------------------------
  //  Scintillator Polystyrene
  //--------------------------------------------------

// energies on top 

// match the energy entries --> 64
std::vector<G4double> refractiveIndexPS = {
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57,
    1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57, 1.57,
    1.57, 1.57, 1.57, 1.57
};

// match the energy entries --> 64
std::vector<G4double> absPS = {
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m,
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m,
    2.6 * m, 2.6 * m, 2.6 * m, 2.6 * m
};

  
  std::vector<G4double> scintilFast(64, 1.0);

  // Add entries into properties table
  G4MaterialPropertiesTable* mptPolystyrene = new G4MaterialPropertiesTable();
  mptPolystyrene->AddProperty("RINDEX", BC404_energy, refractiveIndexPS);
  mptPolystyrene->AddProperty("ABSLENGTH", BC404_energy, absPS);
  mptPolystyrene->AddProperty("SCINTILLATIONCOMPONENT1", BC404_energy, scintilFast);
  mptPolystyrene->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
  mptPolystyrene->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptPolystyrene->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.5 * ns);

  fPolystyrene->SetMaterialPropertiesTable(mptPolystyrene);

// Birks' Constant of the scintillator material, that reduces the light output at high energy deposition	% from http://dx.doi.org/10.1088/1674-1137/34/
  fPolystyrene->GetIonisation()->SetBirksConstant(0.111 * mm / MeV);

  //--------------------------------------------------
  // WLSfiber PMMA 
  //--------------------------------------------------

  elements.push_back("C");
  natoms.push_back(5);
  elements.push_back("H");
  natoms.push_back(8);
  elements.push_back("O");
  natoms.push_back(2);

  density = 1.190 * g / cm3;

  fPMMA = fNistMan->ConstructNewMaterial("PMMA", elements, natoms, density);

  elements.clear();
  natoms.clear();

  std::vector<G4double> refractiveIndexPMMA(64,1.49); // 64 entries 
  std::vector<G4double> absPMMA(64,20.0 *m);

  // Add entries into properties table
  G4MaterialPropertiesTable* mptPMMA = new G4MaterialPropertiesTable();
  mptPMMA->AddProperty("RINDEX", BC404_energy, refractiveIndexPMMA);
  mptPMMA->AddProperty("ABSLENGTH", BC404_energy, absPMMA);

  fPMMA->SetMaterialPropertiesTable(mptPMMA);

  //------------------------------------------------------
  //-------------Teflon Wrapping--------------------------
  //------------------------------------------------------

  elements.push_back("C");
  natoms.push_back(2);
  elements.push_back("H");
  natoms.push_back(4);

  density =0.36*g/cm3;

  fTyvek = fNistMan->ConstructNewMaterial("Tyvek", elements, natoms, density);

  elements.clear();
  natoms.clear();



  //------------------------------------------------------
  //-------------Bialkali Cathode-------------------------
  //------------------------------------------------------

  elements.push_back("Sb");
  natoms.push_back(1);
  elements.push_back("Rb");
  natoms.push_back(1);
  elements.push_back("Cs");
  natoms.push_back(1);

  density = 3*g/cm3;

  fBialkaliCathode = fNistMan->ConstructNewMaterial("BialkaliCathode", elements, natoms, density);

  elements.clear();
  natoms.clear();





}
