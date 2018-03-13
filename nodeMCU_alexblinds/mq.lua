-- ###############################################################
-- mq.lua - general functions to publish to MQTT.
--
-- Code modified by ADB
--
-- Original from 
-- https://learn.adafruit.com/diy-esp8266-home-security-with-lua-and-mqtt/programming-the-esp8266-with-lua
-- Phil Moyer
-- Adafruit
-- May 2016
--
-- This code is open source, released under the BSD license. All
-- redistribution must include this header.
-- ###############################################################

-- Handle for creating an object-like environment.  Returned at end of code.
local module = {}

-- ###############################################################
-- Global variables and parameters.
-- ###############################################################

deviceID = "blinds_bedroom"    -- a sensor identifier for this device
mqttHost = "m14.cloudmqtt.com" -- target host (broker)
mqttPort = 19658               -- target port (broker listening on)
mqttUserID = "lwklhoog"        -- account to use to log into the broker
mqttPass = "c5iYiGyizaaa"      -- broker account password
mqttTimeOut = 120              -- connection timeout
dataInt = 300                  -- data transmission interval in seconds
topicQueue = "/house/masterbedroom/rollerblind" -- the MQTT topic queue to use

-- Simple counter, to count number of publishes
counter = 0

-- ###############################################################
-- Functions
-- ###############################################################

-- Function pubEvent() is what we want to happen every publication interval

function pubEvent()
	counter = counter + 1
	pubValue = "Uptime (minutes): " .. dataInt / 60.0 * counter
    print("Publishing to " .. topicQueue .. ": " .. pubValue)   -- print a status message
    mqttBroker:publish(topicQueue, pubValue, 0, 0)  -- publish
end


-- Reconnect to MQTT when we receive an "offline" message.

function reconn()
    print("Disconnected, reconnecting....")
    conn()
end


-- Establish a connection to the MQTT broker with the configured parameters.
function conn()
    print("Connecting to MQTT broker")
    mqttBroker:connect(mqttHost, mqttPort, 0, function(client) print ("connected") end, function(client, reason) print("failed reason: "..reason) end)
end


-- Call this first! --
-- mqtt_start() instantiates the MQTT control object, sets up callbacks,
-- connects to the broker, and then uses the timer to send sensor data.
-- This is the "main" function in this library. This should be called 
-- from init.lua (which runs on the ESP8266 at boot), but only after
-- it's been vigorously debugged.
--
-- Note: once you call this from init.lua the only way to change the
-- program on your ESP8266 will be to reflash the NodeCMU firmware! 

local function mqtt_start()
    -- Instantiate a global MQTT client object
    print("Instantiating mqttBroker")
    mqttBroker = mqtt.Client(deviceID, mqttTimeOut, mqttUserID, mqttPass, 1)
    -- Set up the event callbacks
    print("Setting up callbacks")
    mqttBroker:on("connect", function(client) print ("connected") end)
    mqttBroker:on("offline", reconn)

    -- Connect to the Broker
    conn()

    -- Use the watchdog to call our sensor publication routine
    -- every dataInt seconds to send the sensor data to the 
    -- appropriate topic in MQTT.
    tmr.alarm(0, (dataInt * 1000), 1, pubEvent)
end


function module.start()
	mqtt_start()
end


-- ###############################################################
-- "Main"
-- ###############################################################

-- Return module so that the functions can be accessed in an object-like way
return module
