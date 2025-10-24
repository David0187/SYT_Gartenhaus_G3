import serial
import tkinter as tk

# ===== Serial Port anpassen =====
ser = serial.Serial('COM5', 115200, timeout=1)  # COM-Port deines ESP32

# ===== Tkinter-Fenster erstellen =====
root = tk.Tk()
root.title("Gewaechshaus Sensorwerte")
root.geometry("300x150")

# Labels für die Werte
temp_label = tk.Label(root, text="Temp: -- °C", font=("Helvetica", 16))
temp_label.pack(pady=10)

hum_label = tk.Label(root, text="Hum: -- %", font=("Helvetica", 16))
hum_label.pack(pady=10)

soil_label = tk.Label(root, text="Soil: --", font=("Helvetica", 16))
soil_label.pack(pady=10)

# Funktion zum Aktualisieren der Werte
def update_values():
    line = ser.readline().decode('utf-8').strip()
    if line:
        try:
            temp, hum, soil = line.split(',')
            temp_label.config(text=f"Temp: {float(temp):.1f} °C")
            hum_label.config(text=f"Hum: {float(hum):.1f} %")
            soil_label.config(text=f"Soil: {int(soil)}")
        except:
            pass
    root.after(500, update_values)  # alle 0,5 Sekunden aktualisieren

# Start der Aktualisierung
update_values()

# Tkinter Event-Loop starten
root.mainloop()