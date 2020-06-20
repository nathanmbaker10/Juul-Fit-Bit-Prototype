import tkinter as tk
import serial
import serial.tools.list_ports
import sys

default_port = "/dev/tty.ESP32-ESP32SPP"
baud= 115200
timeout = 0.25
refresh_period = 20 #ms

class Application(tk.Frame):
	def __init__(self, master=None):
		super().__init__(master)
		self.master = master
		self.pack()
		self.create_widgets()

	def create_widgets(self):
		self.connect = tk.Button(self)
		self.connect["text"] = "Begin Connection with ESP32"
		self.connect["command"] = self.openSerial
		self.connect.pack(side="top")
		self.quit = tk.Button(self, text="DISCONNECT",fg="red",command=self.closeSerial)
		self.quit.pack(side="bottom")


	def closeSerial(self):
		try:
			ser.close()
			print("Closed serial...")
		except AttributeError:
			print("Did not work")
			pass
		except serial.SerialException as e:
			print(e)	

	def openSerial(self): 
		global ser
		print("Opening Serial Communication...")
		ser = serial.Serial(port=default_port, baudrate=baud, timeout=timeout)
		if (ser.is_open):
			refresh() # Begin refreshing proccess
			print("Connected to ESP32!")
		else:
			print("Connection Failed")

def refresh():
	try:
		sys.stdout.flush()
		updateSer()
		root.after(refresh_period, refresh)
	except Exception as e:
		print(e)
		raise(e)

def updateSer():
	if not ser.is_open:
		print("Serial Connection Unexpectedly Closed")
		return
	if (ser.in_waiting>0):
		serIn = ser.read(1).decode("ascii")[0]
		print(serIn)

root = tk.Tk(className='Juul Serial COM')
root.geometry("500x200")
app = Application(master=root)
app.mainloop()
