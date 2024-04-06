import pandas as pd
# go into output_files format, and open the csv file
filename = 'Mary_Output_1'
filepath_in = 'output_files/' + filename + '.csv'

# open the csv file into a pandas dataframe
midi_df = pd.read_csv(filepath_in)
print ("\nStart of MIDI File:") # each row represents a midi event
print (midi_df.head()) # each row represents a midi event

# metadata_df contain all midi events with a channel_event header with value of 'Header'
header_row = (midi_df[(midi_df['channel_event'] == ' Header')])
time_sig_row = (midi_df[(midi_df['channel_event'] == ' Time_signature')])
tempo_row = (midi_df[(midi_df['channel_event'] == ' Tempo')])
title_row = (midi_df[(midi_df['channel_event'] == ' Title_t')])
program_change_row = (midi_df[(midi_df['channel_event'] == ' Program_c')])
end_of_file_row = (midi_df[(midi_df['channel_event'] == ' End_of_file')])

# add all metadata rows to metadata_df
metadata_df = header_row
metadata_df = pd.concat([metadata_df, time_sig_row])
metadata_df = pd.concat([metadata_df, tempo_row])
metadata_df = pd.concat([metadata_df, title_row])
metadata_df = pd.concat([metadata_df, program_change_row])
metadata_df = pd.concat([metadata_df, end_of_file_row])

# print metadata_df
print ("\nMetadata:")
print (metadata_df) # each row represents a midi event

# remove metadata_df from midi_df
gh_df = midi_df.drop(metadata_df.index)

# drop the "ignore" column and "velocity" column and "channel_num" column and "track" column
gh_df = gh_df.drop(columns=['track', 'ignore', 'velocity', 'channel_num'])

################################
### Create Piano Frequencies ### 
################################
# Ref: https://en.wikipedia.org/wiki/Piano_key_frequencies
# - Piano Note Range: 1 to 88 is MIDI Note Range: 21 to 88... a difference of 20 notes 
# - A4 is Piano Note 49 which is MIDI Note 69

# create piano notes from midi_notes, add as new column to gh_df
gh_df['piano_note'] = gh_df['midi_note'] - 20 # 20 is the difference between MIDI Note and Piano Note

# create piano frequencies from piano notes, add as new column to gh_df
gh_df['piano_freq'] = (2**((gh_df['piano_note'] - 49) / 12)) * 440

# round all piano frequencies to integers
gh_df['piano_freq'] = gh_df['piano_freq'].round(0)


#########################
### PRINTING NEW FILE ### 
#########################

# replace all NaN with *~* in entire df
gh_df = gh_df.fillna('*~*')

print ("\nNew Start of MIDI File:") # each row represents a midi event
print(gh_df) # each row represents a midi event

# export to csv in output_files directory
filepath_out = 'output_files/' + filename + '_gh.csv'
gh_df.to_csv(filepath_out, index=False)
print ("\nExported to: " + filepath_out)