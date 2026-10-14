import sparkplug_b as sparkplug
import sparkplug_b_pb2
import mqtt
import getMacAddress
import getIpAddress

DEBUG = True


class Client(mqtt.Client):

    def __init__(self, client_id, group_id, node_name, device_name, reboot_callback, process_events_in_thread=True):
        super(Client, self).__init__(client_id, group_id, node_name, device_name, process_events_in_thread)
        self.client.on_message = self.on_message
        self.topics_values = {}
        self.reboot_callback = reboot_callback

    def connect(self, broker_ip_address,
                broker_username=None,
                broker_password=None,
                initial_topic_values=None,
                broker_port=1883,
                keepalive=60):

        self.topics_values = initial_topic_values
        death_payload = sparkplug.getNodeDeathPayload()
        death_byte_array = bytearray(death_payload.SerializeToString())
        self.client.will_set("spBv1.0/" + self.group_id + "/NDEATH/" + self.node_name, death_byte_array, 0,
                             False)

        super(Client, self).connect(broker_ip_address,
                                    broker_username,
                                    broker_password,
                                    broker_port,
                                    keepalive)

        self.publish_births()

    def publish(self, topic, payload):

        if self.broker_connected:
            try:
                # save mqtt topic and payload for device birth
                self.topics_values[topic] = payload
                topic, payload = self.convert_device_payload_to_sparkplug_device_payload(topic, payload)
                message_info = self.client.publish(topic=topic, payload=payload, qos=1, retain=True)
                self.process_events()
                message_info.wait_for_publish()
                self.process_events()
            except:
                pass

    def on_message(self, client, userdata, message):

        tokens = message.topic.split("/")

        if tokens[0] == "spBv1.0" and tokens[1] == self.group_id and tokens[2] == "NCMD" \
                and tokens[3] == self.node_name:
            inbound_payload = sparkplug_b_pb2.Payload()
            inbound_payload.ParseFromString(message.payload)
            for metric in inbound_payload.metrics:
                if metric.name == "Node Control/Next Server":
                    # 'Node Control/Next Server' is an NCMD used to tell the device/client application to
                    # disconnect from the current MQTT server and connect to the next MQTT server in the
                    # list of available servers.  This is used for clients that have a pool of MQTT servers
                    # to connect to.
                    self.publish_births()
                elif metric.name == "Node Control/Rebirth":
                    # 'Node Control/Rebirth' is an NCMD used to tell the device/client application to resend
                    # its full NBIRTH and DBIRTH again.  MQTT Engine will send this NCMD to a device/client
                    # application if it receives an NDATA or DDATA with a metric that was not published in the
                    # original NBIRTH or DBIRTH.  This is why the application must send all known metrics in
                    # its original NBIRTH and DBIRTH messages.
                    self.publish_births()
                elif metric.name == "Node Control/Reboot":
                    # 'Node Control/Reboot' is an NCMD used to tell a device/client application to reboot
                    # This can be used for devices that need a full application reset via a soft reboot.
                    # In this case, we fake a full reboot with a republishing of the NBIRTH and DBIRTH
                    # messages.
                    self.reboot_callback()
                    # self.publish_births()
                else:
                    print "Unknown command: " + metric.name

        else:
            print "Unknown command..."

    def publish_births(self):

        if self.broker_connected:
            self.publish_node_birth()
            self.publish_device_birth()

    def publish_node_birth(self):

        if self.broker_connected:
            # Create the node birth payload
            payload = sparkplug.getNodeBirthPayload()

            # Set up the Node Controls
            sparkplug.addMetric(payload, "Node Control/Next Server", None, sparkplug.MetricDataType.Boolean,
                                False)
            sparkplug.addMetric(payload, "Node Control/Rebirth", None, sparkplug.MetricDataType.Boolean, False)
            sparkplug.addMetric(payload, "Node Control/Reboot", None, sparkplug.MetricDataType.Boolean, False)

            ip_address = getIpAddress.getIpAddress()
            sparkplug.addMetric(payload, "Device Info/ip_address", None, sparkplug.MetricDataType.String,
                                ip_address)

            exit_code, wired_mac_address = getMacAddress.getMacAddress(lanType='wire')
            sparkplug.addMetric(payload, "Device Info/mac_address", None,
                                sparkplug.MetricDataType.String,
                                wired_mac_address)

            sparkplug.addMetric(payload, "Device Info/mqtt_version", None,
                                sparkplug.MetricDataType.String,
                                "3.1.1")

            # Publish the node birth certificate
            byte_array = bytearray(payload.SerializeToString())
            self.client.publish("spBv1.0/" + self.group_id + "/NBIRTH/" + self.node_name, byte_array, 0, False)
            self.process_events()

    def publish_device_birth(self):

        if self.broker_connected:
            topic, payload = self.convert_topics_published_to_sparkplug_device_birth_payload()
            # Publish the initial data with the Device BIRTH certificate
            self.client.publish(topic=topic, payload=payload, qos=0, retain=False)
            self.process_events()

    def on_connect(self, client, userdata, flags, rc):

        super(Client, self).on_connect(client, userdata, flags, rc)

        if self.broker_connected:
            # Subscribing in on_connect() means that if we lose the connection and
            # reconnect then subscriptions will be renewed.
            client.subscribe("spBv1.0/" + self.group_id + "/NCMD/" + self.node_name + "/#")
            client.subscribe("spBv1.0/" + self.group_id + "/DCMD/" + self.node_name + "/#")
            self.process_events()

    def convert_device_payload_to_sparkplug_device_payload(self, topic, payload):
        # determine payload type based on python data type of payload argument
        # default payload type to string initially
        payload_type = sparkplug.MetricDataType.String
        if type(payload) is float:
            payload_type = sparkplug.MetricDataType.Float
        elif type(payload) is int:
            payload_type = sparkplug.MetricDataType.Int32
        elif type(payload) is bool:
            payload_type = sparkplug.MetricDataType.Boolean

        outbound_payload = sparkplug.getDdataPayload()

        sparkplug.addMetric(outbound_payload, topic, None, payload_type, payload)
        sparkplug_payload = bytearray(outbound_payload.SerializeToString())

        sparkplug_topic = "spBv1.0/" + self.group_id + "/DDATA/" + self.node_name + "/" + self.device_name
        return sparkplug_topic, sparkplug_payload

    def convert_topics_published_to_sparkplug_device_birth_payload(self):

        outbound_payload = sparkplug.getDeviceBirthPayload()

        for topic in self.topics_values:

            payload_type = sparkplug.MetricDataType.String
            if type(self.topics_values[topic]) is float:
                payload_type = sparkplug.MetricDataType.Float
            elif type(self.topics_values[topic]) is int:
                payload_type = sparkplug.MetricDataType.Int32
            elif type(self.topics_values[topic]) is bool:
                payload_type = sparkplug.MetricDataType.Boolean

            sparkplug.addMetric(outbound_payload, topic, None, payload_type, self.topics_values[topic])

        sparkplug_payload = bytearray(outbound_payload.SerializeToString())
        sparkplug_topic = "spBv1.0/" + self.group_id + '/DBIRTH/' + self.node_name + "/" + self.device_name
        return sparkplug_topic, sparkplug_payload
