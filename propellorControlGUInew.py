import tkinter as tk
from tkinter.constants import END
import serial # pip install pyserial
import os
import csv
import time

# Function to send data to the Arduino
def send_data():
    user_input = input_box.get()
    sendln(user_input)

def send_slider():
    slider_value = slider.get()
    data = f"P{slider_value}"
    sendln(data)

def send_tare():
    sendln("TARE")

def toggle_enabled():
    global enabled
    if enabled:
        disable()
    else:
        start_test()

def disable():
    global enabled, csv_writer

    sendln("P0")

    if not enabled:
        return

    csv_writer = None
    csv_file.close()

    new_csv_prefix = csv_out_box.get().split(".")[0].rstrip("123456789")

    slider.set(0)
    slider.configure(state="disabled")

    csv_out_box.configure(state="normal")
    csv_out_box.delete(0, END)
    csv_out_box.insert(0, find_next_csv(new_csv_prefix))
    csv_out_lbl.configure(text="About to write to: ")
    tare_btn.configure(state="normal")

    big_btn.configure(text="Start")

    enabled = False

def start_test():
    global enabled, start_time, csv_file, csv_writer, csv_out_box

    csv_file = open(csv_out_box.get(), mode="w")
    csv_writer = csv.writer(csv_file)

    slider.configure(state="normal")
    csv_out_box.configure(state="disabled")
    csv_out_lbl.configure(text="Currently writing to: ")
    tare_btn.configure(state="disabled")
    big_btn.configure(text="Stop (or press space)")

    enabled = True
    start_time = time.time()

def find_next_csv(csv_prefix="output"):
    file_dir = os.path.dirname(os.path.abspath(__file__))
    file_list = os.listdir(file_dir)
    maxfound = 0
    for fname in file_list:
        if fname[-4:] != ".csv":
            continue

        if fname.startswith(csv_prefix):
            existingNum = fname.rstrip(".csv").lstrip(csv_prefix)
            try:
                existingNum = int(existingNum)
            except:
                continue
            maxfound = existingNum if existingNum > maxfound else maxfound

    return f"{csv_prefix}{maxfound+1}.csv"

def handle_key(event):
    if event.char==" ":
        print("SPACE PRESSED")
        disable()

def sendln(line):
    arduino.write(f"{line}\n".encode())
    print(f" TX: {line}")

# Get the Arduino ID from the user
arduino_id = input("Enter the Arduino ID: ")

# Create a serial connection to the Arduino
arduino = serial.Serial(arduino_id, 9600, timeout = 0.2)

enabled = False
start_time = 0.0
csv_file = None
csv_writer = None

# === GUI SETUP ===

# Create the main window
window = tk.Tk()
window.bind("<KeyPress>", handle_key)

top_frame = tk.Frame(window)
top_frame.pack()

bottom_frame = tk.Frame(window)
bottom_frame.pack(side=tk.BOTTOM)

# Create a slider & send btn
slider_frame = tk.Frame(top_frame)
slider = tk.Scale(slider_frame, from_=0, to=100, orient=tk.HORIZONTAL, state="disabled")
slider_btn = tk.Button(slider_frame, text="Send Slider Value", command=send_slider)
slider.pack()
slider_btn.pack()

# Create a user input box & send bttn
input_frame = tk.Frame(top_frame)
input_box = tk.Entry(input_frame)
input_btn = tk.Button(input_frame, text="Send Text", command=send_data)
input_box.pack()
input_btn.pack()

# Stopwatch
stopwatch_frame = tk.Frame(top_frame)
stopwatch_title = tk.Label(stopwatch_frame, text="Elapsed Time (s):")
stopwatch_elapsed = tk.Label(stopwatch_frame, text="0.0")
stopwatch_title.pack()
stopwatch_elapsed.pack()

# pack the frames
slider_frame.pack(side=tk.LEFT)
input_frame.pack(side=tk.LEFT)
stopwatch_frame.pack(side=tk.LEFT)

# tare btn
tare_btn = tk.Button(bottom_frame, text="Tare", command=send_tare)
tare_btn.pack()

# writing to csv input
csv_out_frame = tk.Frame(bottom_frame)
csv_out_box = tk.Entry(csv_out_frame)
csv_out_lbl = tk.Label(csv_out_frame, text="About to write to: ")
csv_out_box.insert(0, find_next_csv())
csv_out_lbl.pack(side=tk.LEFT)
csv_out_box.pack(side=tk.LEFT)
csv_out_frame.pack()

# start & stop bttn
big_btn = tk.Button(bottom_frame, text="Start", command=toggle_enabled)
big_btn.pack(fill=tk.X)

data = bytearray()
while True:
    # Recieve serial data:

    newData = arduino.readline()
    data.extend(newData)

    elapsed_time = round(time.time() - start_time, 3) if enabled else 0.0
    stopwatch_elapsed.configure(text=f"{elapsed_time}")

    if len(newData) == 0:
        # Failed to read anything. Arduino probably off
        print("WARNING: Failed to read any data from arduino")
        time.sleep(0.05)
    elif chr(newData[-1]) != '\n':
        # Failed to read a complete line.
        # Go back to the start of the loop and try to read rest of line
        print(newData[-1])
        print("WARNING: Incomplete line recieved")
        continue
    else:
        # data is now a complete line!
        line = data.decode('utf-8').strip()

        print(f"RX:  {line}")

        parsed = line.split(',')
        if enabled:
            try:
                percentage = float(parsed[0])
                reading_kg = float(parsed[1])
                csv_writer.writerow([elapsed_time, percentage, reading_kg])
            except ValueError:
                print(f"Error in parsing data: {line}")

        # reset data
        data = bytearray()
        

    # Update display / send data:
    window.update_idletasks()
    window.update()
