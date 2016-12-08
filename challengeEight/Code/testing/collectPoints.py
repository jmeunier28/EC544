'''

Script to collect points to train on based on 
BU wifi access point signal strength
Will write all points to a csv file to use for training

command to get access points is: sudo iwlist wlan0 scanning | egrep 'Cell |Quality|ESSID'

THINGS THAT WE STILL NEED FOR THIS TO WORK:
1. a function to call our main and then write whatever data we want into a table in a csv
    file.
2.  This function should be called in a loop so we can collect a bunch of points at each spot 
    Also need a function that parses quality based on name (ESSID??) only so we are collecting
    from reliable source each time
3. For writing to the csv we can use pandas. I have that installed on the pi already

AFTER WE COLLECT AND TRAIN:
1. will need a smaller more simpler script that we can run continuously. Where we 
load the model, collect the point at the spot, predict and then write this predicition
through serial to the Arduino for Austin's code. 
2. I have working code that does all this with RSSI values but if this works it will
hopefully be faster and more accurate


'''


import re
import os
import sys
import subprocess

results = []

# Below are functions to parse the properties of each AP (cell)
# They take one argument, the bunch of text describing one cell in iwlist
# scan and return a property of that cell.

def get_name(cell):
    return matching_line(cell,"ESSID:")[1:-1]

def get_quality(cell):
    quality = matching_line(cell,"Quality=").split()[0].split('/') #gets the first number that is being divided by 
    return str(int(round(float(quality[0]) / float(quality[1]) * 100))).rjust(3) + " %"

def get_channel(cell):
    return matching_line(cell,"Channel:") #idk if we will want this

def get_signal_level(cell):
    # Signal level is on same line as Quality data not totally sure if 
    # this will work might need debug on the pi
    return matching_line(cell,"Quality=").split("Signal level=")[1]


def get_encryption(cell):
    enc=""
    if matching_line(cell,"Encryption key:") == "off":
        enc="Open"
    else:
        for line in cell:
            matching = match(line,"IE:")
            if matching!=None:
                wpa=match(matching,"WPA Version ")
                if wpa!=None:
                    enc="WPA v."+wpa
        if enc=="":
            enc="WEP"
    return enc

def get_address(cell):
    return matching_line(cell,"Address: ")

# Here's a dictionary of rules that will be applied to the description of each
# cell. The key will be the name of the column in the table. The value is a
# function defined above.

rules={"Name":get_name,
       "Quality":get_quality,
       "Channel":get_channel,
       "Encryption":get_encryption,
       "Address":get_address,
       "Signal":get_signal_level
       }

# Here you can choose the way of sorting the table. sortby should be a key of
# the dictionary rules.

def sort_cells(cells):
    sortby ="Address"# "Quality"
    reverse = True
    cells.sort(None, lambda el:el[sortby], reverse)

# You can choose which columns to display here, and most importantly in what order. Of
# course, they must exist as keys in the dict rules.

columns=["Name","Address","Quality","Signal", "Channel","Encryption"]


#below does the work of collecting the data 


def matching_line(lines, keyword):
    """Returns the first matching line in a list of lines. See match()"""
    for line in lines:
        matching=match(line,keyword)
        if matching!=None:
            return matching
    return None

def match(line,keyword):
    """If the first part of line (modulo blanks) matches keyword,
    returns the end of that line. Otherwise returns None"""
    line=line.lstrip()
    length=len(keyword)
    if line[:length] == keyword:
        return line[length:]
    else:
        return None

def parse_cell(cell):
    """Applies the rules to the bunch of text describing a cell and returns the
    corresponding dictionary"""
    parsed_cell={}
    for key in rules:
        rule=rules[key]
        parsed_cell.update({key:rule(cell)})
    return parsed_cell

def print_table(table):
    widths=map(max,map(lambda l:map(len,l),zip(*table))) #functional magic

    justified_table = []
    for line in table:
        justified_line=[]
        for i,el in enumerate(line):
            justified_line.append(el.ljust(widths[i]+2))
        justified_table.append(justified_line)
    
    for line in justified_table:
        for el in line:
            print el,
        print

def print_cells(cells):
    table=[columns]
    for cell in cells:
        cell_properties=[]
        for column in columns:
            cell_properties.append(cell[column])
        table.append(cell_properties)
    print_table(table)

def main():
    """Pretty prints the output of iwlist scan into a table"""
    
    cells=[[]]
    parsed_cells=[]

    proc = subprocess.Popen(["iwlist", "wlan0", "scan"],stdout=subprocess.PIPE, universal_newlines=True)
    out, err = proc.communicate()

    for line in out.split("\n"):
        cell_line = match(line,"Cell ")
        if cell_line != None:
            cells.append([])
            line = cell_line[-27:]
        cells[-1].append(line.rstrip())

    cells=cells[1:]

    for cell in cells:
        parsed_cells.append(parse_cell(cell))

    sort_cells(parsed_cells)
    print_cells(parsed_cells)

main()