import ROOT
import matplotlib.pyplot as plt

# Open the root file
file = ROOT.TFile.Open("WLS_energy.root")
file2 = ROOT.TFile.Open("WLS_hits.root")

# Get the tree named "WLS"
tree = file.Get("WLS")
tree2 = file2.Get("Hits")

# Define variables to store branch values
energy = []
detectorID = []

opticalphotons = []
eventID = []
hits = []

# Loop over the entries in the tree
for i in range(tree.GetEntries()):
    # Get the i-th entry
    tree.GetEntry(i)
    
    # Multiply the energy value by 10^6 and append to the list
    energy.append(tree.Energy * 1e6)
    detectorID.append(tree.DetectorID)

# Close the file
file.Close()

# Loop over the entries in the tree
for i in range(tree2.GetEntries()):
    # Get the i-th entry
    tree2.GetEntry(i)
    
    # Only append if opticalphotons is not equal to 0
    if tree2.OpticalPhotons != 0:
        opticalphotons.append(tree2.OpticalPhotons)
        eventID.append(tree2.EventID)
        hits.append(tree2.Hits)

# Close the file
file2.Close()


# Plot histogram
plt.hist(energy, bins=50, color='blue', alpha=0.7)
plt.xlabel('Energy of photons [eV]')
plt.ylabel('Frequency')
plt.title('Energy of Photons detected by All photodetector')
plt.grid(True)

# Save histogram as PNG
plt.savefig('energy_photons.png')


plt.figure()
 
# Plot histogram of opticalphotons
plt.hist(hits, bins=50, color='red', alpha=0.7)
plt.xlabel('Number of Hits detector')
plt.ylabel('Frequency')
#plt.yscale('log')
#plt.xscale('log')
plt.title('Histogram of Hits')
plt.grid(True)

# Save histogram as PNG
plt.savefig('hits_histogram.png')

#Get the number of entries in the opticalphotons list
# num_entries = len(hits)
# print("Number of entries in opticalphotons:", num_entries)
