import tkinter as tk
import serial
import serial.tools.list_ports
import sys
import subprocess
import firebase_admin
from firebase_admin import credentials, firestore
import google

fbase_id = "u1"
cred = credentials.Certificate("../Private/JuulServiceAccountKey.json") # IMPORTANT, relative to where main.py is compiled
default_app = firebase_admin.initialize_app(cred)
db = firestore.client()
doc_ref = db.collection('users').document(fbase_id)

default_port = "/dev/tty.ESP32-ESP32SPP"
baud= 115200
timeout = 0.25
refresh_period = 1 #ms

def getCurrentUserData():
	try: 
		userDoc = doc_ref.get()
		userDict = userDoc.to_dict()
		return (userDict["hits"], userDict["double-taps"],userDict["cartridge-in"])
	except google.cloud.exceptions.NotFound:
		print("ERROR: Couldn't find user data. Check internet connection.")


def updateUserData(hits,doubleTaps,cartridgeIn):
	doc_ref.set({
		'hits':hits,
		'double-taps':doubleTaps,
		'cartridge-in':cartridgeIn
	})

def handleUserEvent(event):
	hits, doubleTaps, cartridgeMode = getCurrentUserData()
	if event == 0:
		print("JUUL: Noise Bytes")
	elif event == 1:
		cartridgeMode = False
		print("JUUL: Cartridge Removed")
	elif event == 2:
		cartridgeMode = True
		print("JUUL: Cartridge Inserted")
	elif event == 3:
		doubleTaps += 1
		print("JUUL: Double Tap Detected")
	elif event == 4:
		hits += 1
		print("JUUL: Hit Detected")
	elif event == 5:
		print("JUUL: Excessive Time")
	else:
		print("ERROR: Random integer transmitted")
	updateUserData(hits, doubleTaps, cartridgeMode)

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
		if 'ser' in globals() and ser.is_open :
			try:
				ser.close()
				print("Closed serial...")
			except AttributeError:
				print("Did not work")
				pass
			except serial.SerialException as e:
				print(e)	
		else:
			print("There is no current ESP32 connection, can't close.")

	def openSerial(self): 
		global ser
		print("Opening Serial Communication...")
		try: 
			ser = serial.Serial(port=default_port, baudrate=baud, timeout=timeout) # Defaulted to 8n1
			self.refresh()
		except Exception as e:
			print("Connection Failed, Please Check Bluetooth")			

	def refresh(self):
		try:
			sys.stdout.flush()
			self.updateSer()
		except Exception as e:
			print(e)
			raise(e)

	def updateSer(self):
		if not ser.is_open:
			print("Serial Connection Closed")
			return
		else:
			root.after(refresh_period, self.refresh)

			if (ser.in_waiting>0):
				serIn = ser.read(1).decode("ascii")[0]
				handleUserEvent(int(serIn))


root = tk.Tk(className='Juul Serial COM')
root.geometry("300x150")
app = Application(master=root)
app.mainloop()
