import time
import math
import matplotlib.pyplot as plt

data_file_path = "output.txt"  # Replace with the path to your data file

# Save the parsed data to lists for matplotlib
temperature = []
humidity = []
solar = []
photo = []

# Read data_file_path
# Returns a list(whole file) of lists (each line)
def read_data():
    with open(data_file_path, 'r') as file:
        lines = file.readlines()
    
    data = []
    for line in lines:
        try:
            row = list(map(int, line.split()))
            data.append(row)
        except ValueError as e:
            print(f"INFO: Skipping invalid line: {line.strip()}. Error: {e}")

    return data

# Converse data to make it human readable
def process_data():
    # Every run reads the whole file and overwrites previous values
    data = read_data()
    temperature.clear()
    humidity.clear()
    solar.clear()
    photo.clear()
    
    for i in range(len(data)):
        for j in range(len(data[0])):
            temperature.append(-39.6 + 0.01*data[i][0])
            humidity.append(-2.0468+ (0.0367 * data[i][1]) + ((-1.5955*(10**(-6)))*(data[i][1])**2))
            photo.append(data[i][2]*2.288818)
            solar.append(data[i][3]*0.281616)

def plot_temp():

    # Plot corresponding list with label
    plt.plot(temperature, label="Temperature",color="green")
    
    # Label and legend
    plt.xlabel('Time(s)')
    plt.ylabel('°C')
    #plt.legend()
    plt.title('Temperature')
    plt.grid(True)
    plt.savefig('static/images/plot_temp.png')  # Save the plot as a static file for flask
    
    #Clear plot after saving to png file
    plt.clf()
    plt.cla()

def plot_hum():

    # Plot corresponding list with label
    plt.plot(humidity, label="Humidity",color="blue")
    
    # Label and legend
    plt.xlabel('Time(s)')
    plt.ylabel('Relative Humidity (%)')
    #plt.legend()
    plt.title('Humidity')
    plt.grid(True)
    plt.savefig('static/images/plot_hum.png')  # Save the plot as a static file for flask
    
    #Clear plot after saving to png file
    plt.clf()
    plt.cla()

def plot_photo():

    # Plot corresponding list with label
    plt.plot(photo, label="Visible light",color="yellow")
    
    # Label and legend
    plt.xlabel('Time(s)')
    plt.ylabel('Photosynthetically Active Radiation')
    #plt.legend()
    plt.title('Visible light')
    plt.grid(True)
    plt.savefig('static/images/plot_photo.png')  # Save the plot as a static file for flask
    
    #Clear plot after saving to png file
    plt.clf()
    plt.cla()

def plot_solar():

    # Plot corresponding list with label
    plt.plot(solar, label="Infrared light",color="red")
    
    # Label and legend
    plt.xlabel('Time(s)')
    plt.ylabel('Solar Radiation')
    #plt.legend()
    plt.title('Infrared light')
    plt.grid(True)
    plt.savefig('static/images/plot_solar.png')  # Save the plot as a static file for flask
    
    #Clear plot after saving to png file
    plt.clf()
    plt.cla()



if __name__ == '__main__':
    while True:
        process_data()
        plot_temp()
        plot_hum()
        plot_photo()
        plot_solar()
        time.sleep(10)


