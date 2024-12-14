import ROOT
import matplotlib.pyplot as plt

# Open the root file
file = ROOT.TFile.Open("MAPP_2.root")

# Get the tree named "WLS"
tree = file.Get("WLS")

# Define variables to store branch values
energy = []
detectorID = []
eventID = []

# Loop over the entries in the tree
for i in range(tree.GetEntries()):
    # Get the i-th entry
    tree.GetEntry(i)
    
    # Multiply the energy value by 10^6 and append to the list
    energy.append(tree.Energy * 1e6)
    detectorID.append(tree.DetectorID)
    eventID.append(tree.EventID)


# Close the file
file.Close()

# Plot histogram
plt.hist(energy, bins=50, color='blue', alpha=0.7)
plt.xlabel('Energy of photons [eV]')
plt.ylabel('Frequency')
plt.title('Energy of Photons detected by All photodetector')
plt.grid(True)

# Save histogram as PNG
plt.savefig('energy_photons.png')

# Show the plot (optional)
#plt.show()

plt.figure()

# Plot histogram
plt.hist(detectorID, bins=5, color='red', alpha=0.7)
plt.xlabel('Photodetector ID')
plt.ylabel('Frequency')
#plt.yscale('log')
plt.title('Optical photons detected by each photodetector [20 events]')
plt.grid(True)

# Save histogram as PNG
plt.savefig('detector.png')

# Show the plot (optional)
#plt.show()

# Filter the detectorID list for values equal to 1
# detectorID_0 = [id for id in detectorID if id == 0.0]
# detectorID_1 = [id for id in detectorID if id == 1]
# detectorID_2 = [id for id in detectorID if id == 2]
# detectorID_3 = [id for id in detectorID if id == 3]

# Count the occurrences of value 1 in the detectorID list
count_detectorID_0 = detectorID.count(0)
count_detectorID_1 = detectorID.count(1)
count_detectorID_2 = detectorID.count(2)
count_detectorID_3 = detectorID.count(3)


# Calculate the ratio
avg0 = count_detectorID_0 / 20
avg1 = count_detectorID_1 / 20
avg2 = count_detectorID_2 / 20
avg3 = count_detectorID_3 / 20



print("Avg photons detected at detectorID equal to 0:", avg0)
print("Avg photons detected at detectorID equal to 1:", avg1)
print("Avg photons detected at detectorID equal to 2:", avg2)
print("Avg photons detected at detectorID equal to 0:", avg3)


# # Calculate the average
# average_detectorID_0 = sum(detectorID_0) / 100
# average_detectorID_1 = sum(detectorID_1) / 100
# average_detectorID_2 = sum(detectorID_2) / 100
# average_detectorID_3 = sum(detectorID_3) / 100

# print("Average value of detectorID where its value is equal to 0:", average_detectorID_0)
# print("Average value of detectorID where its value is equal to 1:", average_detectorID_1)
# print("Average value of detectorID where its value is equal to 2:", average_detectorID_2)
# print("Average value of detectorID where its value is equal to 3:", average_detectorID_3)

# # Display event IDs
# print("Event IDs:")
# for event in eventID:
#     print(event)

from collections import Counter

    # Count the repeated energy values
energy_counts = Counter(energy)

# Display the counts
print("Repeated energy values:")
for energy_value, count in energy_counts.items():
    if count > 1:
        print(f"Energy: {energy_value}, Count: {count}")