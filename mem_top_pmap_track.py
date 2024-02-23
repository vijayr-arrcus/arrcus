#!/usr/bin/python3
import json
import os
import sys
import time
from datetime import datetime

st=1800
spyder_proc_details_list = []

def setup_proc_list ():
    os.system("spyder status > /tmp/spyder_status.json")
    f = open('/tmp/spyder_status.json')
    data = json.load(f)
    for i in data:
        proc_details={"name":None, "pid":None}
        proc_details['name'] = i['name']
        proc_details['pid'] = i['pid']
        if i['pid']:
            spyder_proc_details_list.append(proc_details)

def setup_pmap_folder():
    now = datetime.now()
    dt_string = now.strftime("%d_%m_%Y_%H_%M_%S")
    global pmap_folder_name
    pmap_folder_name = "/tmp/pmap_"+dt_string
    os.system("mkdir "+pmap_folder_name)
    for i in spyder_proc_details_list:
        proc_folder = pmap_folder_name+"/"+i['name']+"_"+str(i['pid'])
        os.system("mkdir "+proc_folder)

def main (argv):
    setup_proc_list()
    setup_pmap_folder()
    top_file=pmap_folder_name+"/top.txt"
    i=1
    while True:
        now = datetime.now()
        dt_string = now.strftime("%d_%m_%Y_%H_%M_%S")
        cmd = "echo ============================================================= >> "+top_file
        os.system(cmd)
        cmd = "echo {}.start >> {}".format(str(i), top_file)
        os.system(cmd)
        cmd = "echo  {} >> {}".format(dt_string, top_file)
        os.system(cmd)
        cmd = "echo ------------------------------------------ >> "+top_file
        os.system(cmd)
        cmd = "free >> "+top_file
        os.system(cmd)
        cmd = "echo ------------------------------------------ >> "+top_file
        os.system(cmd)
        cmd = "top -n 1 -b o %MEM >>"+top_file
        os.system(cmd)
        cmd = "echo {}.end >> {}".format(str(i), top_file)
        os.system(cmd)
        cmd = "echo ============================================================= >> "+top_file
        os.system(cmd)

        for proc in spyder_proc_details_list:
            proc_folder = pmap_folder_name+"/"+proc['name']+"_"+str(proc['pid'])
            proc_file = "{}/{}_pmap.log".format(proc_folder, str(i))
            cmd = "echo  {} >> {}".format(dt_string, proc_file)
            os.system(cmd)
            cmd = "pmap -XX {} >> {}".format(proc['pid'], proc_file)
            os.system(cmd)

        i += 1
        print("Goind to sleep "+str(i))
        time.sleep(st)

if __name__ == "__main__":
    main(sys.argv[1:])
