#   FILE: webscraper.py
#   AUTHOR: Jamol Yunusov
#   COMPANY: (c) Bridge Analayzers, All Rights Reserved.

import os
import csv
import sys
import tkinter
import datetime
import requests
import threading
from tkinter import *
from bs4 import BeautifulSoup
from tkinter import messagebox


# Tkinter window
main = tkinter.Tk()
main.title("Bridge Analyzers")
main.geometry('400x300+750+350')
main.resizable(0 ,0)
main.config(bg='black')

# scrape label
scrape_lbl = Label(main, bg='black', fg='DodgerBlue', text="Web Scraper", font=('Helvetica', 18, 'bold'))
scrape_lbl.pack()

# date/time lablel
time_lbl = Label(main, bg='black', fg='white', text='', font=('Helvetica', 15, 'bold'))
time_lbl.pack()

date_lbl = Label(main, bg='black', fg='white', text='', font=('Helvetica', 15, 'bold'))
date_lbl.pack()

# co2 label
co2_lbl = Label(main, bg='black', fg='white', text="CO2 % 0.00", font=('Helvetica', 18, 'bold'))
co2_lbl.place(x=135, y=120)

# info label
info_lbl = Label(main, bg='DodgerBlue', text="Important: 'Stop' program to save scraped data.\rData will be lost otherwise.", font=('Helvetica', 13, 'bold'))
info_lbl.place(x=8, y=240)

# stop gui/program
stop_btn = Button(main, text="Stop", fg='red2', width=11, font=('Helvetica', 15), command=main.quit)
stop_btn.place(x=135, y=180)


# main script
def run_main():

    # Create co2_logs directory
    dir_name = "co2_logs"
    try:
        os.mkdir(dir_name) # make directory
    except:
        pass # directory already exists

    # Create new log file with date and time
    date = datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
    co2_logs = open(f"{dir_name}/log_{date, sys.argv[1]}.csv", mode="w", newline='')
    co2_writer = csv.writer(co2_logs) # write to csv
    co2_writer.writerow(["Timestamp", "CO2 %"]) # header

    while True:
        try:
            response = requests.get('http://' + sys.argv[1], verify=True, timeout=5) # making HTTP GET requests to a specified URL
            response.raise_for_status() # If the response was successful, no Exception will be raised

            soup = BeautifulSoup(response.text, 'html.parser') # create object
            co2_data = soup.find('h4', attrs={"class":"co2_data"}) # find h4

            strip_co2 = co2_data.text.strip('\r\n') # removing leading and trailing whitespaces

            co2_lbl['text'] = "CO2 % " + strip_co2 # display value on gui
            
            for get_co2 in co2_data:
                co2_writer.writerow([datetime.datetime.now().strftime("%m-%d-%Y-%H:%M:%S %p"), strip_co2])
                #print(get_co2.text)
        except requests.Timeout:
            messagebox.showerror("Error", "Timeout Error")
            main.quit()
            break
        except requests.HTTPError or requests.exceptions.ConnectionError:
            messagebox.showerror("Error", "HTTP Error Occured")
            main.quit()
            break
        except:
            break


# display date/time
def date_time():
    now = datetime.datetime.now()
    time_lbl['text'] = now.strftime('%H:%M:%S %p')
    date_lbl['text'] = datetime.datetime.now().strftime('%m-%d-%Y')
    main.after(1000, date_time)
date_time()


# threading main program
thread_main = threading.Thread(target=run_main)
thread_main.start()

# execute main loop
main.mainloop()
sys.exit()
# # # # # # # # # # # # # # # # # # # # # # # # # # # # 
