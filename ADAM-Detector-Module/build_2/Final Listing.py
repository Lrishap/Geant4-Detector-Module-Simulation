import ROOT

# Open the root file
file = ROOT.TFile.Open("WLS_energy.root")

# Get the tree named "WLS"
tree = file.Get("WLS")

# Define variables to store branch values
detectorID = []
eventID = []

# Loop over the entries in the tree
for i in range(tree.GetEntries()):
    # Get the i-th entry
    tree.GetEntry(i)
    
    # Append detector ID and event ID values to their respective lists
    detectorID.append(tree.DetectorID)
    eventID.append(tree.EventID)

# Close the file
file.Close()

# Find unique EventIDs
unique_event_ids = list(set(eventID))

# Count occurrences of each detector ID for each event ID
all_detector_id_counts = {}
for unique_event_id in unique_event_ids:
    # Extract detector IDs for the current unique EventID
    detector_ids_for_current_event = [detectorID[i] for i in range(len(eventID)) if eventID[i] == unique_event_id]
    
    # Count occurrences of each detector ID for the current event ID
    detector_id_counts = {}
    for detector_id in detector_ids_for_current_event:
        if detector_id in detector_id_counts:
            detector_id_counts[detector_id] += 1
        else:
            detector_id_counts[detector_id] = 1
    
    # Store detector ID counts for the current event ID
    all_detector_id_counts[unique_event_id] = detector_id_counts

# Write detector ID counts for each event ID to a text file
with open("detector_id_counts_all_events.txt", "w") as f:
    for event_id, detector_id_counts in all_detector_id_counts.items():
        f.write(f"Event ID: {event_id}\n")
        for detector_id, count in detector_id_counts.items():
            f.write(f"Detector ID {detector_id}: {count}\n")

print("Detector ID counts for all events written to detector_id_counts_all_events.txt file.")
