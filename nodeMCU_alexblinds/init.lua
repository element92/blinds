-- file : init.lua

print("Alex's Rollerblinds")
print("===================")

mq          = require("mq")
wifi_setup  = require("wifi_setup")

wifi_setup.start()

mq.start()

-- 
