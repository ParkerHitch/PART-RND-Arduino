import tkinter as tk
import serial

# Function to send data to the Arduino
def send_data():
    user_input = input_box.get()
    slider_value = slider.get()
    data = f"{user_input},{slider_value}\n"
    arduino.write(data.encode())

# Create the main window
window = tk.Tk()

# Create a user input box
input_box = tk.Entry(window)
input_box.pack()

# Create a slider
slider = tk.Scale(window, from_=0, to=100, orient=tk.HORIZONTAL)
slider.pack()

# Create a button to send the data
send_button = tk.Button(window, text="Send", command=send_data)
send_button.pack()

# stop_data currently undefined
#stop_button = tk.button(window, text="Stop", command=stop_data)


# Get the Arduino ID from the user
arduino_id = input("Enter the Arduino ID: ")

# Create a serial connection to the Arduino
arduino = serial.Serial(arduino_id, 9600)  # Replace 'COM3' with the appropriate port and baud rate

# Start the GUI event loop
window.mainloop()