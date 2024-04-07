from paho.mqtt import client as mqtt_client
import logging
import random
import time
import pandas as pd

# Logging, Constants
FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

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
    filename = 'Mary_Output_1_gh.csv'
    filepath_in = 'output_files/' + filename
    csv_headers = ['start_tick', 'channel_event', 'midi_note', 'piano_note', 'piano_freq', 'dynamic_note', 'tick_duration', 'end_tick']
    midi_gh = pd.read_csv(filepath_in, names=csv_headers)

    # for each line in midi_gf, send a message to MQTT PUBLISH TOPIC
    for index, row in midi_gh.iterrows():
        print(row['start_tick'], row['channel_event'], row['midi_note'], row['piano_note'], row['piano_freq'], row['dynamic_note'], row['tick_duration'], row['end_tick'])

        # send a message to MQTT PUBLISH TOPIC
        time.sleep(1)
        # create a MQTT message of the current row's piano_freq and the next 3 rows piano freqs
        msg = str(row['piano_freq']) + ',' + str(midi_gh.iloc[index + 1]['piano_freq']) + ',' + str(midi_gh.iloc[index + 2]['piano_freq']) + ',' + str(midi_gh.iloc[index + 3]['piano_freq'])
        result = client.publish(topic, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Send `{msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")
         
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