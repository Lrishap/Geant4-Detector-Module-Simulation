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
/// \file optical/wls/src/WLSDetectorConstruction.cc
/// \brief Implementation of the WLSDetectorConstruction class
//
//

#include "WLSDetectorConstruction.hh"

#include "WLSDetectorMessenger.hh"
#include "WLSMaterials.hh"
#include "WLSPhotonDetSD.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4EllipticalTube.hh"
#include "G4ios.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PhysicalConstants.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"


#include <iostream> // Include the necessary header for std::cout

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSDetectorConstruction::WLSDetectorConstruction()
  : fVisAttributes()
  , fMaterials(nullptr)
  , fLogicHole(nullptr)
  , fLogicWorld(nullptr)
  , fPhysiWorld(nullptr)
  , fPhysiHole(nullptr)
{
  fDetectorMessenger = new WLSDetectorMessenger(this);

  fNumOfCladLayers = 2;

  fSurfaceRoughness = 1;

  fMirrorToggle       = false; //switch mirror off 
  fMirrorPolish       = 1.;
  fMirrorReflectivity = 1.;

  fMPPCPolish       = 1.;
  fMPPCReflectivity = 0.;

  fExtrusionPolish       = 1.;
  fExtrusionReflectivity = 1.;

  fXYRatio = 1.0;




  fCoatingThickness = 0.11 * mm;

//  fCoatingThickness = 0. * mm; // testing



  fScintThickness = (1.27*cm/2 + fCoatingThickness)*2;  // this is overall thickness of scintillator 
  // due to code it computes as - fCoatingThickness as extrusion as thickness 


 // fScintThickness = 1.27*cm; 


  fWLSfiberZ      = fScintThickness/2;  //length of WLS fibre
  fWLSfiberRY     = 0.44 * mm; 
  fWLSfiberOrigin = 0* cm;    // z position placement of WLS for booking purpose
  fWLSTotal = 1* mm;  // this is total diameter of WLS --> used for cladding 


  fMPPCShape = "Square";  // Photodetector square shapped 
  //fMPPCHalfL = fWLSfiberRY;
  //fMPPCHalfL = 0.5*mm; //testing purpose 
  fMPPCHalfL = 3* mm; //size of PMT


  fMPPCDist  = 0.00 * mm; // no gap 
  fMPPCTheta = 0.0 * deg;
  fMPPCZ     = 0.05 * mm; // thickness of detector+

  fClrfiberZ = fMPPCZ + 10. * nm;
  fMirrorZ   = 0.1 * mm;


  // coating and hole radius 
  fHoleRadius       = fWLSfiberRY + 0.03*2*fWLSTotal;    // cladding + wls
  // fCoatingRadius    = 0.11* mm;



// changing to scintillator tiles
  fBarLength        =  fScintThickness;
  fBarBase          = 0.500 * m + fCoatingThickness*2;
  fHoleLength       = fBarLength;          //+ fCoatingThickness;
  fCoatingRadius    = 0 * mm;



  fWLShiftPosition = fCoatingThickness;                     // position of hole shifted
  fPDPos = fWLSfiberZ+fMPPCZ + fCoatingThickness; // position of PD 


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WLSDetectorConstruction::~WLSDetectorConstruction()
{
  if(fDetectorMessenger)
    delete fDetectorMessenger;
  if(fMaterials)
    delete fMaterials;
  for (auto visAttributes: fVisAttributes)
  {
    delete visAttributes;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* WLSDetectorConstruction::Construct()
{
  if(fPhysiWorld)
  {
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    G4LogicalSkinSurface::CleanSurfaceTable();
    G4LogicalBorderSurface::CleanSurfaceTable();
  }

  fMaterials = WLSMaterials::GetInstance();
  UpdateGeometryParameters();

  return ConstructDetector();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* WLSDetectorConstruction::ConstructDetector()
{

  auto air = FindMaterial("G4_AIR");
  //G4cout << "\nMaterial Properties Table for G4_AIR:" << G4endl;
  //air->GetMaterialPropertiesTable()->DumpTable();

  //--------------------------------------------------
  // World
  //--------------------------------------------------

  G4VSolid* solidWorld =
    new G4Box("World", fWorldSizeX, fWorldSizeY, fWorldSizeZ);

  fLogicWorld =
    new G4LogicalVolume(solidWorld, air, "World");

  fPhysiWorld =
    new G4PVPlacement(0, G4ThreeVector(), fLogicWorld, "World", 0, false, 0);

  //--------------------------------------------------
  // Extrusion
  //--------------------------------------------------

// older code for cf. 

//   auto coating = FindMaterial("Coating");
//   G4VSolid* solidExtrusion = new G4Box("Extrusion", GetBarBase() / 2.,GetBarBase() / 2., GetBarLength() / 2.);
//   G4LogicalVolume* logicExtrusion =new G4LogicalVolume(solidExtrusion, coating, "Extrusion");
//   G4OpticalSurface* TiO2Surface = new G4OpticalSurface("TiO2Surface", unified, ground, dielectric_metal, fExtrusionPolish); //dielectric_metal
//   G4MaterialPropertiesTable* TiO2SurfaceProperty = new G4MaterialPropertiesTable();
//   std::vector<G4double> p_TiO2 = { 2.00 * eV, 3.47 * eV };
//   std::vector<G4double> refl_TiO2 = { fExtrusionReflectivity, fExtrusionReflectivity };
//   std::vector<G4double> effi_TiO2 = { 0., 0. };

//   std::vector<G4double> rindex_Teflon = {1.54, 1.54};

//  // TiO2SurfaceProperty->AddProperty("RINDEX", p_TiO2, rindex_Teflon);
//   TiO2SurfaceProperty->AddProperty("REFLECTIVITY", p_TiO2, refl_TiO2);
//   TiO2SurfaceProperty->AddProperty("EFFICIENCY", p_TiO2, effi_TiO2);
//   TiO2Surface->SetMaterialPropertiesTable(TiO2SurfaceProperty);
//   new G4PVPlacement(0, G4ThreeVector(), logicExtrusion, "Extrusion", fLogicWorld, false, 0);

//   new G4LogicalSkinSurface("TiO2Surface", logicExtrusion, TiO2Surface);



auto coating = FindMaterial("Tyvek");
G4VSolid* solidExtrusion = new G4Box("Extrusion", GetBarBase() / 2.,GetBarBase() / 2., GetBarLength() / 2.);
G4LogicalVolume* logicExtrusion =new G4LogicalVolume(solidExtrusion, coating, "Extrusion");
G4OpticalSurface* TyvekSurface = new G4OpticalSurface("TyvekSurface", unified, ground, dielectric_metal, fExtrusionPolish); //dielectric_metal --> tyvek acts as dielectric_metal in GEANT4

// match the energy photons reaching tyvek layer to generation [scintillator photon energies] --> 64 entries
std::vector<G4double> TyvekOptEne = {
    2.382 * eV, 2.411 * eV, 2.438 * eV, 2.467 * eV, 2.493 * eV, 2.517 * eV, 2.543 * eV, 2.565 * eV, 2.587 * eV, 2.607 * eV,
    2.625 * eV, 2.642 * eV, 2.657 * eV, 2.667 * eV, 2.678 * eV, 2.688 * eV, 2.706 * eV, 2.721 * eV, 2.742 * eV, 2.749 * eV,
    2.76 * eV, 2.776 * eV, 2.791 * eV, 2.804 * eV, 2.805 * eV, 2.814 * eV, 2.822 * eV, 2.828 * eV, 2.831 * eV, 2.832 * eV,
    2.841 * eV, 2.85 * eV, 2.857 * eV, 2.879 * eV, 2.898 * eV, 2.912 * eV, 2.924 * eV, 2.933 * eV, 2.947 * eV, 2.962 * eV,
    2.973 * eV, 2.983 * eV, 2.989 * eV, 3.0 * eV, 3.01 * eV, 3.012 * eV, 3.014 * eV, 3.026 * eV, 3.03 * eV, 3.036 * eV,
    3.044 * eV, 3.05 * eV, 3.06 * eV, 3.071 * eV, 3.081 * eV, 3.094 * eV, 3.104 * eV, 3.12 * eV, 3.145 * eV, 3.166 * eV,
    3.205 * eV, 3.26 * eV, 3.332 * eV, 3.432 * eV
};

std::vector<G4double>  refractiveIndexTyvek(64, 1.50);
std::vector<G4double>  SpecularLobeTyvek(64,0.85);
std::vector<G4double>  SpecularSpikeTyvek(64,0.0); 
std::vector<G4double>  BackscatterTyvek(64,0.0);
std::vector<G4double>  ReflectivityTyvek(64,1.0);
std::vector<G4double>  effi_Tyvek(64,0.0);


G4MaterialPropertiesTable *TyvekPropertiesTable = new G4MaterialPropertiesTable();

  // Optical Properties
TyvekPropertiesTable->AddProperty("RINDEX", TyvekOptEne, refractiveIndexTyvek);
TyvekPropertiesTable->AddProperty("SPECULARLOBECONSTANT", TyvekOptEne, SpecularLobeTyvek);
TyvekPropertiesTable->AddProperty("SPECULARSPIKECONSTANT", TyvekOptEne, SpecularSpikeTyvek);
TyvekPropertiesTable->AddProperty("BACKSCATTERCONSTANT", TyvekOptEne, BackscatterTyvek);
TyvekPropertiesTable->AddProperty("REFLECTIVITY", TyvekOptEne, ReflectivityTyvek);
TyvekPropertiesTable->AddProperty("EFFICIENCY", TyvekOptEne, effi_Tyvek);

TyvekSurface->SetMaterialPropertiesTable(TyvekPropertiesTable);

new G4PVPlacement(0, G4ThreeVector(), logicExtrusion, "Extrusion", fLogicWorld, false, 0);
new G4LogicalSkinSurface("TyvekSurface", logicExtrusion, TyvekSurface);



  //--------------------------------------------------
  // Scintillator
  //--------------------------------------------------

  auto polystyrene = FindMaterial("Polystyrene");
  //G4cout << "\nMaterial Properties Table for Polystyrene:" << G4endl;
  //polystyrene->GetMaterialPropertiesTable()->DumpTable();

  G4VSolid* solidScintillator =
    new G4Box("Scintillator",
              GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius(),  //coating on x 
              GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius(),  //coating on y 
              GetBarLength() / 2. - GetCoatingThickness() );                   // coating on z                        

  G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator, polystyrene, "Scintillator");

  if(fCoatingThickness<=0.0){
  new G4PVPlacement(0, G4ThreeVector(), logicScintillator, "Scintillator", fLogicWorld, false, 0);
  }
  else{
  new G4PVPlacement(0, G4ThreeVector(), logicScintillator, "Scintillator", logicExtrusion, false, 0);
  }

  if(GetFiberRadius() <= GetHoleRadius())
  {
    G4VSolid* solidHole = new G4Tubs(
      "Hole", 0., GetHoleRadius(), GetHoleLength() / 2., 0. * deg, 360. * deg);

    fLogicHole = new G4LogicalVolume(solidHole, air, "Hole");


    G4double pmtSpacing = 5.0 * cm; // Spacing between PMTs

// // 5x5 = 25 holes WLS placement loop 
for (G4int l = 0; l < 2; ++l) {
    for (G4int m = 0; m < 2; ++m) {
      for (G4int i = 0; i < 6; ++i) {
         for (G4int j = 0; j < 6; ++j) {
      // Calculate the position for each hole
        G4double x2pos = 0.1 * cm;
        G4double y2pos = 0.1 * cm;
        G4double yPMT = -2.5 * cm + pmtSpacing * l;
        G4double zPMT = 2.5 * cm - pmtSpacing * m;

        //std::cout << "WLS ID : " << (l * 2 + m) * 25 + i * 5 + j << std::endl;   // debug check 

       // hole 
       fPhysiHole = new G4PVPlacement(0, G4ThreeVector(yPMT + -0.25* cm + x2pos * i, zPMT + 0.25 * cm - y2pos * j, fWLShiftPosition), fLogicHole, "Hole", logicScintillator, false, (l * 2 + m) * 30 + i * 6 + j); // Set the copy number from 0 to 24     
     }
    }
  }
}
}

  //--------------------------------------------------
  // Fiber
  //--------------------------------------------------

  if(!(fLogicHole) || !(fPhysiHole))
  {
    G4ExceptionDescription ed;
    ed << "The Fiber Hole has not been constructed";
    G4Exception("WLSDetectorConstruction", "wls001", FatalException, ed);
  }

  // Pointers to the most recently constructed volume
  G4LogicalVolume* logicPlacement   = fLogicHole;
  G4VPhysicalVolume* physiPlacement = fPhysiHole;

  //--------------------------------------------------
  // Fiber Construction
  //--------------------------------------------------

  // Boundary Surface Properties
  G4OpticalSurface* opSurface = nullptr;

  if(fSurfaceRoughness < 1.)
    opSurface = new G4OpticalSurface("RoughSurface", glisur, ground,
                                     dielectric_dielectric, fSurfaceRoughness);
  
  G4LogicalVolume* logicWLSfiber = nullptr;
  G4LogicalVolume* logicClad1    = nullptr;
  G4LogicalVolume* logicClad2    = nullptr;
  G4VPhysicalVolume* physiClad1  = nullptr;
  G4VPhysicalVolume* physiClad2  = nullptr;

  auto fpethylene = FindMaterial("FPethylene");
  auto pethylene = FindMaterial("Pethylene");
  auto pmma = FindMaterial("PMMA");
  auto fWLSPolystyrene = FindMaterial("WLSPolystyrene");

  // Determine the number of cladding layers to be built
  switch(fNumOfCladLayers)
  {
    case 2:

      //--------------------------------------------------
      // Cladding 2
      //--------------------------------------------------

      //G4cout << "\nMaterial Properties Table for fPethylene:" << G4endl;
      //fpethylene->GetMaterialPropertiesTable()->DumpTable();

      G4VSolid* solidClad2;

      if(fXYRatio == 1.)
        solidClad2 = new G4Tubs("Clad2", 0., fClad2RX, fClad2Z, 0., twopi);
      else
        solidClad2 = new G4EllipticalTube("Clad2", fClad2RX, fClad2RY, fClad2Z);

      logicClad2 =
        new G4LogicalVolume(solidClad2, fpethylene, "Clad2");

      physiClad2 =
        new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, fWLSfiberOrigin),
                          logicClad2, "Clad2", logicPlacement, false, 0);

      // Place the rough surface only if needed
      if(opSurface)
      {
        new G4LogicalBorderSurface("surfaceClad2Out", physiClad2,
                                   physiPlacement, opSurface);
        new G4LogicalBorderSurface("surfaceClad2In", physiPlacement, physiClad2,
                                   opSurface);
      }

      logicPlacement = logicClad2;
      physiPlacement = physiClad2;
      [[fallthrough]];

    case 1:

      //--------------------------------------------------
      // Cladding 1
      //--------------------------------------------------

      //G4cout << "\nMaterial Properties Table for Pethylene:" << G4endl;
      //pethylene->GetMaterialPropertiesTable()->DumpTable();

      G4VSolid* solidClad1;

      if(fXYRatio == 1.)
        solidClad1 = new G4Tubs("Clad1", 0., fClad1RX, fClad1Z, 0., twopi);
      else
        solidClad1 = new G4EllipticalTube("Clad1", fClad1RX, fClad1RY, fClad1Z);

      logicClad1 = new G4LogicalVolume(solidClad1, pmma, "Clad1"); //pethylene

      physiClad1 =
        new G4PVPlacement(0, G4ThreeVector(0., 0., fWLSfiberOrigin), logicClad1,
                          "Clad1", logicPlacement, false, 0);

      // Place the rough surface only if needed
      if(opSurface)
      {
        new G4LogicalBorderSurface("surfaceClad1Out", physiClad1,
                                   physiPlacement, opSurface);

        new G4LogicalBorderSurface("surfaceClad1In", physiPlacement, physiClad1,
                                   opSurface);
      }

      logicPlacement = logicClad1;
      physiPlacement = physiClad1;
      [[fallthrough]];

    default:

      //--------------------------------------------------
      // WLS Fiber
      //--------------------------------------------------

      //G4cout << "\nMaterial Properties Table for PMMA:" << G4endl;
      //pmma->GetMaterialPropertiesTable()->DumpTable();

      G4VSolid* solidWLSfiber;

      if(fXYRatio == 1.)
      {
        solidWLSfiber =
          new G4Tubs("WLSFiber", 0., fWLSfiberRX, fWLSfiberZ, 0., twopi);
      }
      else
      {
        solidWLSfiber = new G4EllipticalTube("WLSFiber", fWLSfiberRX,
                                             fWLSfiberRY, fWLSfiberZ);
      }

      logicWLSfiber =
        new G4LogicalVolume(solidWLSfiber, fWLSPolystyrene, "WLSFiber"); //pmma

      logicWLSfiber->SetUserLimits(
        new G4UserLimits(DBL_MAX, DBL_MAX, 10. * ms));

      G4VPhysicalVolume* physiWLSfiber =
        new G4PVPlacement(0, G4ThreeVector(0., 0., fWLSfiberOrigin),
                          logicWLSfiber, "WLSFiber", logicPlacement, false, 0);

      // Place the rough surface only if needed
      if(opSurface)
      {
        new G4LogicalBorderSurface("surfaceWLSOut", physiWLSfiber,
                                   physiPlacement, opSurface);

        new G4LogicalBorderSurface("surfaceWLSIn", physiPlacement,
                                   physiWLSfiber, opSurface);
      }
  }


  //--------------------------------------------------
  // Coupling at the read-out end
  //--------------------------------------------------

  // Clear Fiber (Coupling Layer)
  // G4VSolid* solidCouple = new G4Box("Couple", fCoupleRX, fCoupleRY, fCoupleZ);

  // G4LogicalVolume* logicCouple =
  //   new G4LogicalVolume(solidCouple, air, "Couple");

  // new G4PVPlacement(0, G4ThreeVector(0., 0., fCoupleOrigin), logicCouple,
  //                   "Couple", fLogicWorld, false, 0);

  //--------------------------------------------------
  // A logical layer in front of PhotonDet
  //--------------------------------------------------

  // Purpose: Preventing direct dielectric to metal contact

  // Check for valid placement of PhotonDet
  if(fMPPCTheta > std::atan(fMPPCDist / fMPPCHalfL))
  {
    fMPPCTheta   = 0.;
    fMPPCOriginX = std::sin(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
    fMPPCOriginZ = -fCoupleZ + std::cos(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
    G4ExceptionDescription ed;
    ed << "Invalid alignment.  Alignment reset to 0.";
    G4Exception("WLSDetectorConstruction", "wls002", JustWarning, ed);
  }

  // //--------------------------------------------------
  // PhotonDet (Sensitive Detector)
  //--------------------------------------------------


  auto BialkaliCathode = FindMaterial("BialkaliCathode");


  // Physical Construction
  G4VSolid* solidPhotonDet = nullptr;

  if(fMPPCShape == "Square")
    solidPhotonDet = new G4Box("PhotonDet", fMPPCHalfL, fMPPCHalfL, fMPPCZ);
  else
    solidPhotonDet = new G4Tubs("PhotonDet", 0., fMPPCHalfL, fMPPCZ, 0., twopi);

  G4LogicalVolume* logicPhotonDet = new G4LogicalVolume(solidPhotonDet, BialkaliCathode, "PhotonDet_LV");


G4double pmtSpacing = 5.0 * cm; // Spacing between PMTs
// loop for the sensitive detector Place 4 PMTs within the larger tile
for (G4int i = 0; i < 2; ++i) {
    for (G4int j = 0; j < 2; ++j) {
        // Calculate the position for each PMT
        G4double xPMT = -2.5 * cm + pmtSpacing * i;
        G4double yPMT = 2.5 * cm - pmtSpacing * j;
        G4double zPMT = fPDPos;
      
    //    std::cout << "Copy ID : " << i * 2 + j << std::endl;   // debug check 

        G4PVPlacement* physicalDetector = new G4PVPlacement(0, G4ThreeVector(xPMT, yPMT, zPMT), logicPhotonDet, "PhotonDet", fLogicWorld, false, i * 2 + j); // copy number of PMTs 
    }
}

  // PhotonDet Surface Properties
  G4OpticalSurface* photonDetSurface = new G4OpticalSurface("PhotonDetSurface", glisur, ground, dielectric_metal, fMPPCPolish);

  G4MaterialPropertiesTable* photonDetSurfaceProperty = new G4MaterialPropertiesTable();

 // similair procedure can be utilized for Creating a pseudo SiPm (Hamamtsu S14160-6050HS ) [60 entries]
  std::vector<G4double> p_mppc = {
    1.38 * eV, 1.40 * eV, 1.43 * eV, 1.46 * eV, 1.49 * eV, 1.52 * eV, 
    1.56 * eV, 1.60 * eV, 1.65 * eV, 1.69 * eV, 1.72 * eV, 1.75 * eV, 
    1.80 * eV, 1.84 * eV, 1.88 * eV, 1.92 * eV, 1.93 * eV, 1.97 * eV,
    2.01 * eV, 2.03 * eV, 2.08 * eV, 2.11 * eV, 2.14 * eV, 2.19 * eV, 
    2.23 * eV, 2.28 * eV, 2.33 * eV, 2.38 * eV, 2.43 * eV, 2.47 * eV, 
    2.51 * eV, 2.58 * eV, 2.65 * eV, 2.78 * eV, 2.91 * eV, 3.05 * eV, 
    3.05 * eV, 3.15 * eV, 3.19 * eV, 3.29 * eV, 3.33 * eV, 3.39 * eV, 
    3.52 * eV, 3.63 * eV, 3.74 * eV, 3.81 * eV, 3.95 * eV, 4.01 * eV, 
    4.10 * eV, 4.13 * eV, 4.16 * eV, 4.17 * eV, 4.22 * eV, 4.24 * eV,
    4.24 * eV, 4.25 * eV, 4.29 * eV, 4.31 * eV, 4.36 * eV, 4.45 * eV
};    

//Corresponding Efficiency for the energies above  (Hamamtsu S14160-6050HS )  (upto 3 decimal places)
 std::vector<G4double> effi_mppc = {0.039, 0.047, 0.054, 0.063, 0.077, 0.085, 0.100, 0.109, 0.128, 0.140,
 0.153, 0.165, 0.182, 0.202, 0.212, 0.231, 0.241, 0.257, 0.272, 0.287,
 0.303, 0.316, 0.328, 0.348, 0.362, 0.383, 0.402, 0.421, 0.439, 0.454,
 0.474, 0.494, 0.505, 0.510, 0.498, 0.480, 0.480, 0.464, 0.450, 0.432,
 0.417, 0.401, 0.393, 0.382, 0.369, 0.356, 0.332, 0.311, 0.291, 0.242, 
 0.209, 0.254, 0.187, 0.133, 0.088, 0.112, 0.063, 0.045, 0.028, 0.018};

  std::vector<G4double> refl_mppc(60, 0.0);  

  photonDetSurfaceProperty->AddProperty("REFLECTIVITY", p_mppc, refl_mppc);
  photonDetSurfaceProperty->AddProperty("EFFICIENCY", p_mppc, effi_mppc);

  photonDetSurface->SetMaterialPropertiesTable(photonDetSurfaceProperty);

  new G4LogicalSkinSurface("PhotonDetSurface", logicPhotonDet, photonDetSurface);

  // visualization attributes -------------------------------------------------

  auto visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0,0.0));
  visAttributes->SetVisibility(false);
  fLogicWorld->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(1.0,0.0,0.0,0.5));
  visAttributes->SetVisibility(true);
  logicExtrusion->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);


  visAttributes = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.9));
  visAttributes->SetVisibility(true);
  logicScintillator->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

 // added due to if condition earlier
  // if(GetCoatingRadius() > 0.){
  // visAttributes = new G4VisAttributes(G4Colour(0.0,0.8,0.2,0.2));
  // visAttributes->SetVisibility(true);
  // logicScintSide->SetVisAttributes(visAttributes);
  // fVisAttributes.push_back(visAttributes);

  // visAttributes = new G4VisAttributes(G4Colour(0.0,0.8,0.2,0.2));
  // visAttributes->SetVisibility(true);
  // logicScintCrnr->SetVisAttributes(visAttributes);
  // fVisAttributes.push_back(visAttributes);
  // }

  // visAttributes = new G4VisAttributes(G4Colour(0.4,0.0,0.0,0.5));
  // visAttributes->SetVisibility(true);
  // fLogicHole->SetVisAttributes(visAttributes);
  // fVisAttributes.push_back(visAttributes);

  if(logicClad1 != nullptr)
  {
    visAttributes = new G4VisAttributes(G4Colour(0.0,0.8,0.5,0.5));
    visAttributes->SetVisibility(true);
    logicClad1->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
  }

  if(logicClad2 != nullptr)
  {
    visAttributes = new G4VisAttributes(G4Colour(0.0,0.5,0.8,0.5));
    visAttributes->SetVisibility(true);
    logicClad2->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
  }

  visAttributes = new G4VisAttributes(G4Colour(0.8,0.8,1.0));
  visAttributes->SetVisibility(true);
  logicWLSfiber->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0,0.8));
  visAttributes->SetVisibility(true);
  logicPhotonDet->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);



  return fPhysiWorld;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::ConstructSDandField()
{
  if(!fmppcSD.Get())
  {
    G4String mppcSDName    = "WLS/PhotonDet";
    WLSPhotonDetSD* mppcSD = new WLSPhotonDetSD(mppcSDName);
    G4SDManager::GetSDMpointer()->AddNewDetector(mppcSD);
    fmppcSD.Put(mppcSD);
  }
  SetSensitiveDetector("PhotonDet_LV", fmppcSD.Get(), true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::UpdateGeometryParameters()
{
  fWLSfiberRX = fXYRatio * fWLSfiberRY;

  fClad1RX = fWLSfiberRX + 0.03 * fWLSTotal;
  fClad1RY = fWLSfiberRY + 0.03 * fWLSTotal;
  fClad1Z  = fWLSfiberZ;

  fClad2RX = fClad1RX + 0.03 * fWLSTotal;
  fClad2RY = fClad1RY + 0.03 * fWLSTotal;
  fClad2Z  = fWLSfiberZ;

  fWorldSizeX = 90*cm;
  fWorldSizeY = 90*cm;
  fWorldSizeZ = 90*cm;

  fCoupleRX = fClad2RX + fMPPCDist + fMPPCHalfL + 1. * cm;
  fCoupleRY = fClad2RY + fMPPCDist + fMPPCHalfL + 1. * cm;
  fCoupleZ  = ((fWLSfiberZ + fMPPCDist + fMPPCHalfL + 1. * cm) - fWLSfiberZ) / 2.;

  fClrfiberHalfL = fMPPCHalfL;

  fMirrorRmax = fClad2RY;

 // fCoupleOrigin = fWLSfiberOrigin + fWLSfiberZ + fCoupleZ;

  fCoupleOrigin = fWLSfiberZ + fCoupleZ;


  //fMirrorOrigin = fWLSfiberOrigin - fWLSfiberZ - fMirrorZ;// for mirror 

  fMPPCOriginX  = std::sin(fMPPCTheta) * (fMPPCDist + fClrfiberZ);
  fMPPCOriginZ  = -fCoupleZ + std::cos(fMPPCTheta) * (fMPPCDist + fClrfiberZ);

// Printing out the values
std::cout << "fWorldSizeX: " << fWorldSizeX << std::endl;
std::cout << "fWorldSizeY: " << fWorldSizeY << std::endl;
std::cout << "fWorldSizeZ: " << fWorldSizeZ << std::endl;

std::cout << "fMPPCOriginX: " << fMPPCOriginX << std::endl;
std::cout << "fMPPCOriginZ: " << fMPPCOriginZ << std::endl;


std::cout << "fGetHoleLength(): " << GetHoleLength() << std::endl;
std::cout << "Scintillator X dim: " << GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius()<< std::endl;;
std::cout << "Scintillator Y dim: " <<  GetBarBase() / 2. - GetCoatingThickness() - GetCoatingRadius()<< std::endl;
std::cout << "Scintillator Z dim: " <<  GetBarLength() / 2. - GetCoatingThickness() << std::endl;
std::cout << "Thickness of warp " << GetCoatingThickness() << std::endl;





}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetPhotonDetGeometry(G4String shape)
// Set the Geometry of the PhotonDet detector
// Pre:  shape must be either "Circle" and "Square"
{
  if(shape == "Circle" || shape == "Square")
    fMPPCShape = shape;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetNumberOfCladding(G4int num)
// Set the number of claddings
// Pre: 0 <= num <= 2
{
  fNumOfCladLayers = num;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetWLSLength(G4double length)
// Set the TOTAL length of the WLS fiber
{
  fWLSfiberZ = length;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetWLSRadius(G4double radius)
// Set the Y radius of WLS fiber
{
  fWLSfiberRY = radius;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetClad1Radius(G4double radius)
// Set the Y radius of Cladding 1
{
  fClad1RY = radius;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetClad2Radius(G4double radius)
// Set the Y radius of Cladding 2
{
  fClad2RY = radius;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetPhotonDetHalfLength(G4double halfL)
// Set the half length of the PhotonDet detector
// The half length will be the radius if PhotonDet is circular
{
  fMPPCHalfL = halfL;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetGap(G4double gap)
// Set the distance between fiber end and PhotonDet
{
  fMPPCDist = gap;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetPhotonDetAlignment(G4double theta)
// Set the Aligment of PhotonDet with respect to the z axis
// If theta is 0 deg, then the s perfectly aligned
// PhotonDet will be deviated by theta from z axis
// facing towards the center of the fiber
{
  fMPPCTheta = theta;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetSurfaceRoughness(G4double roughness)
// Set the Surface Roughness between Cladding 1 and WLS fiber
// Pre: 0 < roughness <= 1
{
  fSurfaceRoughness = roughness;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetMirrorPolish(G4double polish)
// Set the Polish of the mirror, polish of 1 is a perfect mirror surface
// Pre: 0 < polish <= 1
{
  fMirrorPolish = polish;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetMirrorReflectivity(G4double reflectivity)
// Set the Reflectivity of the mirror, reflectivity of 1 is a perfect mirror
// Pre: 0 < reflectivity <= 1
{
  fMirrorReflectivity = reflectivity;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetPhotonDetPolish(G4double polish)
// Set the Polish of the PhotonDet, polish of 1 is a perfect mirror surface
// Pre: 0 < polish <= 1
{
  fMPPCPolish = polish;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetPhotonDetReflectivity(G4double reflectivity)
// Set the Reflectivity of the PhotonDet, reflectivity of 1 is a perfect mirror
// Pre: 0 < reflectivity <= 1
{
  fMPPCReflectivity = reflectivity;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetMirror(G4bool flag)
// Toggle to place the mirror or not at one end (-z end) of the fiber
// True means place the mirror, false means otherwise
{
  fMirrorToggle = flag;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetXYRatio(G4double r)
// Set the ratio of the x and y radius of the ellipse (x/y)
// a ratio of 1 would produce a circle
{
  fXYRatio = r;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetBarLength(G4double length)
// Set the length of the scintillator bar
{
  fBarLength = length;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetBarBase(G4double side)
// Set the side of the scintillator bar
{
  fBarBase = side;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetHoleRadius(G4double radius)
// Set the radius of the fiber hole
{
  fHoleRadius = radius;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetCoatingThickness(G4double thick)
// Set thickness of the coating on the bars
{
  fCoatingThickness = thick;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WLSDetectorConstruction::SetCoatingRadius(G4double radius)
// Set inner radius of the corner bar coating
{
  fCoatingRadius = radius;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetWLSFiberLength() { return fWLSfiberZ; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetBarLength() { return fBarLength; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetBarBase() { return fBarBase; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetHoleRadius() { return fHoleRadius; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetHoleLength() { return fHoleLength; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetFiberRadius() { return GetWLSFiberRMax(); }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetCoatingThickness()
{
  return fCoatingThickness;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetCoatingRadius() { return fCoatingRadius; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetWLSFiberEnd()
{
  return fWLSfiberOrigin + fWLSfiberZ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetWLSFiberRMax()
{
  if(fNumOfCladLayers == 2)
    return fClad2RY;
  if(fNumOfCladLayers == 1)
    return fClad1RY;
  return fWLSfiberRY;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double WLSDetectorConstruction::GetSurfaceRoughness()
{
  return fSurfaceRoughness;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Return True if the fiber construction is ideal
G4bool WLSDetectorConstruction::IsPerfectFiber()
{
  return fSurfaceRoughness == 1. && fXYRatio == 1. &&
         (!fMirrorToggle || (fMirrorPolish == 1. && fMirrorReflectivity == 1.));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material* WLSDetectorConstruction::FindMaterial(G4String name)
{
  G4Material* material = G4Material::GetMaterial(name, true);
  return material;
}
