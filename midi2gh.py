import pandas as pd
import sys

# go into output_files format, and open the csv file
filename = 'All_Of_Me'
filepath_in = 'output_files/' + filename + '.csv'

DYN_NOTE_COUNT = 5

# open the csv file into a pandas dataframe with columns [track,time,channel_event,channel_num,midi_note,velocity,ignore]
csv_headers = ['track', 'time', 'channel_event', 'channel_num', 'midi_note', 'velocity', 'ignore', 'ignore_2']
midi_df = pd.read_csv(filepath_in, names=csv_headers)

print ("\nStart of MIDI File:") # each row represents a midi event
print (midi_df.head()) # each row represents a midi event

# drop all rows from midi_df that are not 'Start_track', ' Note_on_c', ' Note_off_c', ' End_track'
midi_df = midi_df[(midi_df['channel_event'] == ' Start_track') | 
                  (midi_df['channel_event'] == ' Note_on_c') | 
                  (midi_df['channel_event'] == ' Note_off_c') | 
                  (midi_df['channel_event'] == ' End_track')]

# move all other rows to metadata_df
metadata_df = midi_df[(midi_df['channel_event'] != ' Start_track') &
                        (midi_df['channel_event'] != ' Note_on_c') & 
                        (midi_df['channel_event'] != ' Note_off_c') & 
                        (midi_df['channel_event'] != ' End_track')]

# print metadata_df
print ("\nMetadata:")
print (metadata_df) # each row represents a midi event

# remove metadata_df from midi_df
gh_df = midi_df.drop(metadata_df.index)

# drop the "ignore" column and "velocity" column and "channel_num" column and "track" column
gh_df = gh_df.drop(columns=['track', 'ignore', 'velocity', 'channel_num', 'ignore_2'])

# rename the "time" column to "start_tick"
gh_df = gh_df.rename(columns={'time': 'start_tick'})

################################
### Create Piano Frequencies ### 
################################
# Ref: https://en.wikipedia.org/wiki/Piano_key_frequencies
# - Piano Note Range: 1 to 88 is MIDI Note Range: 21 to 88... a difference of 20 notes 
# - A4 is Piano Note 49 which is MIDI Note 69

# convert gh_df['midi_note'] to int, except for NaN values
gh_df['midi_note'] = gh_df['midi_note'].apply(lambda x: x if pd.isnull(x) else int(x))

# create piano notes from midi_notes, add as new column to gh_df
gh_df['piano_note'] = (gh_df['midi_note']) - 20 # 20 is the difference between MIDI Note and Piano Note

# create piano frequencies from piano notes, add as new column to gh_df
gh_df['piano_freq'] = (2**((gh_df['piano_note'] - 49) / 12)) * 440

# round all piano frequencies to integers
gh_df['piano_freq'] = gh_df['piano_freq'].round(0)

# determine the range of the piano notes in gh_df from column piano_note
piano_note_min = gh_df['piano_note'].min()
piano_note_max = gh_df['piano_note'].max()
piano_note_range = int(piano_note_max) - int(piano_note_min)

# create a list of the unique piano notes played
piano_notes_list = gh_df['piano_note'].unique()
number_unique_piano_notes = gh_df['piano_note'].nunique()

## FIX THIS ENTIRE SECTION ##

# reverse piano_notes_list (notes are now highest to lowest)
piano_notes_list_reversed = piano_notes_list[::-1]

# map each note in piano_notes_list to DYN_NOTE_COUNT dynamically assignable notes in column dynamic_note
# for each unique note in piano_notes_list_reversed, assign a value from 1 to DYN_NOTE_COUNT via modulo DYN_NOTE_COUNT into dict mapped_piano_notes
mapped_piano_notes = {}
for i, note in enumerate(piano_notes_list):
    mapped_piano_notes[note] = piano_notes_list_reversed[i] % DYN_NOTE_COUNT + 1

# add a value of mapped_piano_notes to map "*~*" to "*~*" (for the START and END MIDI events)
mapped_piano_notes['*~*'] = '*~*'
mapped_piano_notes['nan'] = '*~*'
mapped_piano_notes["NaN"] = '*~*'

print(gh_df)
print(piano_notes_list_reversed)
print(mapped_piano_notes)
# sys.exit()

# for each midi event in gh_df, assign a dynamic_note based on the mapped_piano_notes assignment
gh_df['dynamic_note'] = gh_df['piano_note'].map(mapped_piano_notes)

# for only rows with a value of " Note_on_c" for header "channel_event"
# create a new column "duration" which is the difference between the current row's "time" and the next row's "time"
if ' Note_on_c' in gh_df['channel_event'].values:
    gh_df['tick_duration'] = gh_df['start_tick'].shift(-1) - gh_df['start_tick']
else:
    gh_df['tick_duration'] = 0

# delete all rows with a value of " Note_off_c" for header "channel_event"
gh_df = gh_df[gh_df['channel_event'] != ' Note_off_c']

# start_tick of row A + tick_duration of row A SHOULD equal start_tick of row B
# create a new column "end_tick" which is the sum of the current row's "start_tick" and "tick_duration"
gh_df['end_tick'] = gh_df['start_tick'] + gh_df['tick_duration']

#########################
### PRINTING NEW FILE ### 
#########################

# replace all NaN with *~* in entire df
gh_df = gh_df.fillna('*~*')

print ("\nNew Start of MIDI File:") # each row represents a midi event
print(gh_df) # each row represents a midi event

# print the range of the piano notes in gh_df from column piano_note
print ("\nPiano Note Range:", piano_note_min, "to", piano_note_max)
print ("Piano Range of", piano_note_range, "Notes")
print ("Number of Unique Piano Notes:", number_unique_piano_notes)

# export to csv in output_files directory
filepath_out = 'output_files/' + filename + '_gh.csv'
gh_df.to_csv(filepath_out, index=False)
print ("\nExported to: " + filepath_out)

# (1) Add "strumming" 
# (2) Conversion 