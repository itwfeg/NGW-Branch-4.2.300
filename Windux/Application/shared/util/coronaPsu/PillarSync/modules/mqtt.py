import time

import paho.mqtt.client as mqtt_client

DEBUG = True


class Client(object):

    def __init__(self,
                 client_id,
                 group_id,
                 node_name,
                 device_name,
                 process_events_in_thread=True):
        self.client = mqtt_client.Client(client_id=client_id, clean_session=True,
                                         userdata=None,
                                         protocol=mqtt_client.MQTTv311, transport="tcp")
        self.client.on_publish = self.on_publish
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.broker_connected = False
        self.group_id = group_id
        self.node_name = node_name
        self.device_name = device_name
        self.process_events_in_thread = process_events_in_thread
        self.process_events_thread_running = False

    def process_events(self):
        if self.process_events_in_thread:
            if not self.process_events_thread_running:
                self.process_events_thread_running = True
                self.client.loop_start()
        else:
            self.client.loop(timeout=1.0, max_packets=1)

    def set_username_and_password(self, broker_username, broker_password):
        if broker_username and broker_password:
            self.client.username_pw_set(broker_username, broker_password)

    def connect(self,
                broker_ip_address,
                broker_username=None,
                broker_password=None,
                broker_port=1883,
                keepalive=60):

        self.set_username_and_password(broker_username, broker_password)

        self.client.connect(host=broker_ip_address, port=broker_port, keepalive=keepalive)
        self.process_events()
        time.sleep(1.0)

    def broker_is_connected(self):
        return self.broker_connected

    def publish(self, topic, payload):
        if self.broker_connected:
            try:
                payload = str(payload)
                message_info = self.client.publish(topic=topic, payload=payload, qos=1, retain=False)
                self.process_events()
                message_info.wait_for_publish()
                self.process_events()
                if DEBUG:
                    print message_info.rc
            except:
                pass

    def disconnect(self):
        if self.broker_connected:
            self.client.disconnect()
            if self.process_events_thread_running:
                self.process_events_thread_running = False
                self.client.loop_stop()

    def on_message(self, client, userdata, message):
        time.sleep(1.0)
        if DEBUG:
            print "%s, %s" % (message.topic, str(message.payload))

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            if DEBUG:
                print "On Connect:"
                print "Userdata = %s" % userdata
                print "Connection returned result: " + mqtt_client.connack_string(rc)
            self.broker_connected = True
        else:
            if DEBUG:
                print "Failed to connect with result: " + mqtt_client.connack_string(rc)
            self.broker_connected = False

    def on_publish(self, client, userdata, results):
        if DEBUG:
            print "On Publish:"
            print "Client = %s" % client
            print "Userdata = %s" % userdata
            print "Results = %s" % results


if __name__ == "__main__":

    wired_mac_address ="11223344"
    client = Client(client_id=wired_mac_address,
                    group_id="pillarsync_gateways",
                    node_name='pillarsync' + wired_mac_address,
                    device_name='corona_p6000')
    client.connect("127.0.0.1")
    for i in range(0,200):
        client.publish("corona_p6000/power", float(i) * 10.0)
        client.publish("corona_p6000/current", float(i) * 10.0)
        client.publish("corona_p6000/frequency", float(i) * 10.0)
        client.publish("corona_p6000/voltage", float(i) * 10.0)
        if (i % 2) == 0:
            value = True
        else:
            value = False
        client.publish("corona_p6000/fault/high_temp", value)
        client.publish("corona_p6000/fault/interlock", value)
        client.publish("corona_p6000/fault/dc_bus", value)
        client.publish("corona_p6000/fault/overvolt", value)
        client.publish("corona_p6000/fault/current_trip", value)
        client.publish("corona_p6000/fault/inverter_on", value)
    client.disconnect()

