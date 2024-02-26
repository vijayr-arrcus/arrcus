#!/usr/bin/python3
import json
import os
import time
import argparse
from datetime import datetime

st=1800
spyder_proc_details_list = []
mem_dump_root_folder = None

def setup_proc_list ():
    global spyder_proc_details_list

    spyder_proc_details_list.clear() # setup a new list every time
    os.system("spyder status > /tmp/spyder_status.json")
    f = open('/tmp/spyder_status.json')
    data = json.load(f)
    for i in data:
        proc_details={"name":None, "pid":None}
        proc_details['name'] = i['name']
        proc_details['pid'] = i['pid']
        if i['pid']:
            spyder_proc_details_list.append(proc_details)
    f.close()

def setup_proc_pmap_folder():
    global mem_dump_root_folder
    for i in spyder_proc_details_list:
        proc_folder = "{}/{}/".format(mem_dump_root_folder, i['name'])
        os.system("mkdir "+proc_folder)

def setup_mem_dump_folder():
    global mem_dump_root_folder

    if mem_dump_root_folder is None:
        now = datetime.now()
        dt_string = now.strftime("%d_%m_%Y_%H_%M_%S")
        mem_dump_root_folder = "/tmp/pmap_"+dt_string

    cmd = "mkdir {}".format(mem_dump_root_folder)
    os.system(cmd)
    setup_proc_pmap_folder()


def main ():
    global mem_dump_root_folder
    global spyder_proc_details_list
    global st

    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--interval', type=int, dest='interval', help='The interval in seconds at which memory dumps has to be collected (default 1800 secs)')
    parser.add_argument('-f', '--folder',   type=str, dest='folder', help='The folder in which the outputs should be stored')
    args = parser.parse_args()

    if args.interval:
        st=args.interval
    if args.folder:
        mem_dump_root_folder=args.folder

    setup_proc_list()
    setup_mem_dump_folder()
    top_file=mem_dump_root_folder+"/top.txt"

    print("Starting periodic memory collection interval {} secs in location {}".format(st, mem_dump_root_folder))

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
            proc_folder = mem_dump_root_folder+"/"+proc['name']
            proc_file = "{}/{}_pmap.log".format(proc_folder, str(i))
            cmd = "echo  {} >> {}".format(dt_string, proc_file)
            os.system(cmd)
            cmd = "pmap -XX {} >> {}".format(proc['pid'], proc_file)
            os.system(cmd)

        i += 1
        time.sleep(st)
        setup_proc_list() #cache proc list again since some daemons could have restarted

if __name__ == "__main__":
    main()
