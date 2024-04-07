from paho.mqtt import client as mqtt_client
import logging
import random
import time
import pandas as pd
import sys

# Logging, Constants
FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60
DYN_NOTE_COUNT = 4


# Song [0] Mary Had a Little Lamb
# Song [1] All of Me (John Legend)
SONG_TICKS_PER_SECONDS = [352, 960]
filename = 'All_Of_Me_gh'
SONG_NUM = 1


# ref: https://en.wikipedia.org/wiki/Piano_key_frequencies
# create a giant freq_lookup table mapping piano notes (1 to 88) to their frequencies. piano notes represented as index + 1
FREQ_LOOKUP_TABLE = [round((2**((i - 49) / 12)) * 440) for i in range(88)] # 88 piano notes mapped to their frequencies
# print(FREQ_LOOKUP_TABLE[49]) # verify that A4 is 440 Hz

# Credentials
broker = 'broker.emqx.io'
port = 1883
topic = "wumbo/frequency" # wumbo/joystick and wumbo/buttons
client_id = f'python-mqtt-{random.randint(0, 1000)}'

# username = 'emqx'
# password = 'public'

def connect_mqtt():
    def on_connect(client, userdata, flags, rc, properties):
    # For paho-mqtt 2.0.0, you need to add the properties parameter.
    # def on_connect(client, userdata, flags, rc, properties):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    # Set Connecting Client ID
    # (Paho v1)
    # client = mqtt_client.Client(client_id)

    # (Paho v2) For paho-mqtt 2.0.0, you need to set callback_api_version.
    client = mqtt_client.Client(client_id=client_id, callback_api_version=mqtt_client.CallbackAPIVersion.VERSION2)

    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    # client.on_disconnect = on_disconnect
    return client

def publish(client):
    # open output_files Mary_Output_1_gh.csv
    filepath_in = 'output_files/' + filename + '.csv'
    csv_headers = ['start_tick', 'channel_event', 'midi_note', 'piano_note', 'piano_freq', 'dynamic_note', 'tick_duration', 'end_tick']
    midi_gh = pd.read_csv(filepath_in, names=csv_headers)

    # create DYN_NOTE_COUNT size array to represent frequencies of the 4 buttons
    dynamic_button_freqs = [0] * DYN_NOTE_COUNT

    # for each line in midi_gf, send a message to MQTT PUBLISH TOPIC
    for index, row in midi_gh.iterrows():
        print(row['start_tick'], row['channel_event'], row['midi_note'], row['piano_note'], row['piano_freq'], row['dynamic_note'], row['tick_duration'], row['end_tick'])

        # ignore first row (bc they are headers)
        if index == 0:
            continue

        # get current row frequency
        curr_freq = row['piano_freq']
        curr_freq = int(curr_freq)

        # find the curr_freq's index in the FREQ_LOOKUP_TABLE
        freq_lookup_table_idx = FREQ_LOOKUP_TABLE.index(curr_freq)

        # get current row dynamic_button assignment
        curr_dynamic_btn_assignment = row['dynamic_note'] # current button assignment
        curr_dynamic_btn_assignment = 0

        # curr_dynamic_btn_assignment is a value from 0 to 3 and represents an index of dynamic_button_freqs
        # curr_dynamic_btn_assignment maps to freq_lookup_table_idx, so the value at dynamic_button_freqs[curr_dynamic_btn_assignment] should be curr_freq
        # map the other index 
        btn_index = int(curr_dynamic_btn_assignment)
        for i in range(btn_index, 4):
            dynamic_button_freqs[i] = FREQ_LOOKUP_TABLE[freq_lookup_table_idx + i - btn_index]
        for i in range(0, btn_index):
            dynamic_button_freqs[i] = FREQ_LOOKUP_TABLE[freq_lookup_table_idx - btn_index + i]

        # get current row note duration
        curr_note_duration = row['tick_duration']

        # send current_row piano_freq + (3) fake piano_freq + note_duration
        note_msg = str(dynamic_button_freqs[0]) + ',' + str(dynamic_button_freqs[1]) + ',' + str(dynamic_button_freqs[2]) + ',' + str(dynamic_button_freqs[3]) + ',' + str(curr_note_duration) + ',' + str(SONG_TICKS_PER_SECONDS[SONG_NUM])
        print("note_msg: ", note_msg)

        result = client.publish(topic, note_msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Send `{note_msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")

        # sleep in milliseconds
        print(curr_note_duration)
        print(type(curr_note_duration))
        if curr_note_duration != '0.0':
            note_duration = int(curr_note_duration) / int(SONG_TICKS_PER_SECONDS[SONG_NUM])
        else: 
            note_duration = .500
        time.sleep(note_duration)
         
def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

    client.subscribe(topic)
    client.on_message = on_message

def on_disconnect(client, userdata, rc):
    logging.info("Disconnected with result code: %s", rc)
    reconnect_count, reconnect_delay = 0, FIRST_RECONNECT_DELAY
    while reconnect_count < MAX_RECONNECT_COUNT:
        logging.info("Reconnecting in %d seconds...", reconnect_delay)
        time.sleep(reconnect_delay)

        try:
            client.reconnect()
            logging.info("Reconnected successfully!")
            return
        except Exception as err:
            logging.error("%s. Reconnect failed. Retrying...", err)

        reconnect_delay *= RECONNECT_RATE
        reconnect_delay = min(reconnect_delay, MAX_RECONNECT_DELAY)
        reconnect_count += 1
    logging.info("Reconnect failed after %s attempts. Exiting...", reconnect_count)


def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)
    client.loop_stop()

if __name__ == '__main__':
    run()