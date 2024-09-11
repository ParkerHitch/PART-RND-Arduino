import serial
import csv
import time
import argparse 


#I lowkey forgot some of the functionality I was supposed to add but this probably works as intended

def read_serial_data_to_csv(port):
    # Hardcoded parameters for serial communication and output file - might wanna change em idk what yall using
    baudrate = 9600  # Baud rate for serial communication
    timeout = 1  # Timeout for reading from the serial port (in seconds)
    output_csv = "output.csv"  # Output CSV file name
    start_time = time.time()
    try:
        ser = serial.Serial(port, baudrate, timeout=timeout)
        with open(output_csv, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Timestamp", "Percentage (%)", "Reading (kg)"])
            try:
                print(f"Reading data from {port} and writing to {output_csv}...")
                while True:
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        #Im assuming that the data is in "percentage,kg" format with this one
                        #can be changed to accomodate actual readings
                        data = line.split(',')
                        if len(data) == 2:
                            try:
                                percentage = float(data[0])
                                reading_kg = float(data[1])
                                elapsed_time = round(time.time() - start_time, 2)
                                writer.writerow([elapsed_time, percentage, reading_kg])
                                print(f"{elapsed_time}: Percentage = {percentage}%, Load = {reading_kg} kg")
                            except ValueError:
                                print(f"Error in parsing data: {line}")
            except KeyboardInterrupt:
                #Ends the program when you stop it
                print("Data logging stopped by user.")
            finally:
                ser.close()

    except serial.SerialException:
        print("Error: Unable to open the specified serial port.")
        print(f"Time of error: {time.time()-start_time}")
    except Exception as e:
        print(f"An error occurred: {e}")
        print(f"Time of error: {time.time()-start_time}")

if __name__ == "__main__":
    # Command-line argument parsing to specify the serial port
    parser = argparse.ArgumentParser()
    parser.add_argument('port', type=str)
    args = parser.parse_args()
    # Call the function to read data from the serial port and log it to CSV
    read_serial_data_to_csv(args.port)