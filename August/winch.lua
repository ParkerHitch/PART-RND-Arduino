local parentSerial = serial:find_serial(0)
local updown_channel = 10 -- Change this to the RC channel you're using
local drop_channel = 11 -- Change this to the RC channel you're using
local last_drop = 1000

parentSerial:begin(9600)
parentSerial:set_flow_control(0)

function update() -- This is the loop which periodically runs
  local updown_pwm_value = rc:get_pwm(updown_channel) -- Read the PWM value from the RC channel
  local drop_pwm_value =  rc:get_pwm(drop_channel)

  if (drop_pwm_value > 1500 and last_drop < 1500) then
    writestring("D")
  end
  last_drop = drop_pwm_value

  if updown_pwm_value < 1100 then
    writestring("L") -- Send command to lower the winch
    -- gcs:send_text(6, "Lowering winch")
  elseif updown_pwm_value > 1900 then
    writestring("R") -- Send command to raise the winch
    -- gcs:send_text(6, "Raising winch")
  else
    writestring("I") -- Send command to idle
    -- gcs:send_text(6, "Winch idle")
  end

  local avail = parentSerial:available():tofloat()
  if avail > 0 then
    local read = parentSerial:readstring(avail)
    gcs:send_text(6, "Received: " .. read)
  else
    -- gcs:send_text(6, "No message!")
  end

  return update, 50 -- Reschedules the loop at 20Hz
end

function writestring(str)
  for i = 1, str:len() do
    parentSerial:write(string.byte(str,i,i))
  end
end

return update() -- Run immediately before starting to reschedule

