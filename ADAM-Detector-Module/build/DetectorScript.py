############ Abhinab Mukhpadhyay 6/03/2024 #####################
########### 

#This code is skeleton code which operates as such 
# 1. It doesn't include the Optical photons but can be added (option of root file 2 - second root file is included) 
# 2. You may merge with pYROOT.py code for filtering out bad events howevr this is already filtered root file without any bugs i.e independent of it
# 3. It works by uniquely idenitfying the event number and relates it to detectorID 
# 4. I.e example say EventID = 0 is repeated 16 time because 3 hits happened in PMT 1, 4 hits in PMT2 , 5 hits in PMT3 and rest in PMT4. It has 16 recorded 
#    Instances of hits. Then it extracts the detector ID based on length of the EventID which is 16. 
# 5. I have showcased first event by putting unique_event_ids[0] to show you how it works 
# 6. Now you can modify it , generalize it , plot it however you want 



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
eventID = []

# Loop over the entries in the tree
for i in range(tree.GetEntries()):
    # Get the i-th entry
    tree.GetEntry(i)
    
    # Append energy, detectorID, and eventID values to their respective lists
    energy.append(tree.Energy * 1e6)
    detectorID.append(tree.DetectorID)
    eventID.append(tree.EventID)

# Close the files
file.Close()

# Find the first unique EventID
unique_event_ids = list(set(eventID))
first_unique_event_id = unique_event_ids[0]

# Extract detector IDs for the first unique EventID
detector_ids_for_first_event = [detectorID[i] for i in range(len(eventID)) if eventID[i] == first_unique_event_id]

# Plot histogram
plt.hist(detector_ids_for_first_event,bins=5, color='red', alpha=0.7)
plt.xlabel('DetectorID')
plt.ylabel('Frequency')
plt.title(f'Histogram of DetectorID for EventID {first_unique_event_id}')
plt.grid(True)

# Save histogram as PNG
plt.savefig('hits_histogram.png')