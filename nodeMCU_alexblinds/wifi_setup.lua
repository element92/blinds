-- file: wifi_setup.lua

-- Warning from https://nodemcu.readthedocs.io/en/master/en/modules/wifi/
-- The WiFi subsystem is maintained by background tasks 
-- that must run periodically. Any function or task that 
-- takes longer than 15ms (milliseconds) may cause the 
-- WiFi subsystem to crash. To avoid these potential 
-- crashes, it is advised that the WiFi subsystem be 
-- suspended with wifi.suspend() prior to the execution 
-- of any tasks or functions that exceed this 15ms 
-- guideline.

local module = {}

-- List of Wi-Fi SSIDs and passwords
-- The ESP8266 will attempt to connect to any of these
-- that it finds, in order.
SSID = {}
SSID["AccessibleBeige"] = "crafternooner"

-----------------------------------------------
--- Set Variables ---
-----------------------------------------------
--- WIFI CONFIGURATION ---
WIFI_SIGNAL_MODE = wifi.PHYMODE_N

--- IP CONFIG (Leave blank to use DHCP) ---
ESP8266_IP=""
ESP8266_NETMASK=""
ESP8266_GATEWAY=""
-----------------------------------------------


-- Checks to see if the ESP8266 has been assigned an IP address.
-- If so, proceeds to the next step in the program.
local function wifi_wait_ip()  
  if wifi.sta.getip()== nil then
    print("IP unavailable, Waiting...")
  else
    tmr.stop(1)  -- Turns off the timer
    print("\n===================================")
    print("ESP8266 mode is: " .. wifi.getmode())
    print("MAC address is: " .. wifi.ap.getmac())
    print("IP is "..wifi.sta.getip())
    print("====================================")
    
	-- Perform all of this once the WiFi is connected
	--mq.start()
  end
end

-- Connects to Wi-Fi, then continues to test 
-- the connection until an IP address is assigned.
local function wifi_start(list_aps)  
    if list_aps then
        for key,value in pairs(list_aps) do
            if SSID and SSID[key] then
                wifi.setmode(wifi.STATION);
				wifi.setphymode(WIFI_SIGNAL_MODE)
                wifi.sta.config {ssid=key, pwd=SSID[key]}
                wifi.sta.connect()
				
				-- Only if configured not to use DHCP
				if ESP8266_IP ~= "" then
					wifi.sta.setip({ip=ESP8266_IP,netmask=ESP8266_NETMASK,gateway=ESP8266_GATEWAY})
				end

                print("Connecting to " .. key .. " ...")
				
				-- Every 2500 ms, calls wifi_wait_ip to test for the IP address.
                tmr.alarm(1, 2500, 1, wifi_wait_ip)
            end
        end
    else
        print("Error getting AP list")
    end
end

function module.start()  
  print("Configuring Wifi ...")
  wifi.setmode(wifi.STATION);
  wifi.sta.getap(wifi_start)
end

return module
