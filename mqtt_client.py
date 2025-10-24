import serial
import tkinter as tk

# ===== Serial Port anpassen =====
ser = serial.Serial('COM5', 115200, timeout=1)  # COM-Port deines ESP32

# ===== Tkinter-Fenster erstellen =====
root = tk.Tk()
root.title("Gewaechshaus Sensorwerte")
root.geometry("300x180")

# Labels für die Werte
temp_label = tk.Label(root, text="Temp: -- °C", font=("Helvetica", 16))
temp_label.pack(pady=5)

hum_label = tk.Label(root, text="Hum: -- %", font=("Helvetica", 16))
hum_label.pack(pady=5)

soil_label = tk.Label(root, text="Soil: -- %", font=("Helvetica", 16))
soil_label.pack(pady=5)


# Funktion zum Aktualisieren der Werte
def update_values():
    line = ser.readline().decode('utf-8').strip()
    if line:
        try:
            temp, hum, soil = line.split(',')
            temp = float(temp)
            hum = float(hum)
            soil = int(soil)

            # Soil in Prozent umrechnen (0 = trocken, 4095 = nass)
            soil_percent = 100 * (soil / 4095)

            # Labels aktualisieren
            temp_label.config(text=f"Temp: {temp:.1f} °C")
            hum_label.config(text=f"Hum: {hum:.1f} %")
            soil_label.config(text=f"Soil: {soil_percent:.1f} %")
        except:
            pass
    root.after(500, update_values)  # alle 0,5 Sekunden aktualisieren


# Start der Aktualisierung
update_values()

# Tkinter Event-Loop starten
root.mainloop()
