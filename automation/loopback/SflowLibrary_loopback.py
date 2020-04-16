from arctest.base.logger import Logger
from arctest.base.RobotLibrary import RobotLibrary
import time
from sflowCollector import SflowCollector


class SflowLibrary(RobotLibrary):

    def __init__(self):
        super(SflowLibrary, self).__init__()

    def wait_for_hsflowd_stop(self, switch, wait_time=10, iterations=60):
        net_result = False
        count = 0
        while (not net_result and count < iterations):
            response = switch.run_ssh_command("service hsflowd status")
            if "hsflowd is not running" not in response:
                self.rsleep(wait_time, "Waiting for hsflowd to stop")
            else:
                net_result = True
            count += 1
        if not net_result:
            return False
        return True

    def verify_sflow(self):
        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool")

    def verify_sampling_interval(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.counter_sampling_interval = 20
            if sflow_obj.state.counter_sampling_interval != 20:
                raise Exception("Counter Sampling Interval value not reflected in state for {}".format(sflow), False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[12].split("=")
            if (int(a[1]) != 20):
                raise Exception(
                    "Counter Sampling Interval value not reflected in state for {} in hsflowd file".format(sflow),
                    False)
        return True

    def verify_sampling_interval_modification(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.counter_sampling_interval = 20
            if sflow_obj.state.counter_sampling_interval != 20:
                raise Exception(
                    "Counter Sampling Interval value setting to 20 not reflected in state for {}".format(sflow), False)

        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.counter_sampling_interval = 30
            if sflow_obj.state.counter_sampling_interval != 30:
                raise Exception(
                    "Counter Sampling Interval value modification to 30 not reflected in state for {}".format(sflow),
                    False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[12].split("=")
            if (int(a[1]) != 30):
                raise Exception(
                    "Counter Sampling Interval value not reflected in state for {} in hsflowd file".format(sflow),
                    False)
        return True

    def verify_sampling_interval_deletion(self, switchlist, sflowlist, default_counter_sampling_interval):
        commands = ["config", "no sflow global counter-sampling-interval", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            if sflow_obj.state.counter_sampling_interval != default_counter_sampling_interval:
                raise Exception("Counter Sampling Interval value not reflected in state for {}".format(sflow), False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[12].split("=")
            if (int(a[1]) != default_counter_sampling_interval):
                raise Exception(
                    "Counter Sampling Interval value not reflected in state for {} in hsflowd file".format(sflow),
                    False)
        return True

    def verify_sampling_rate(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.packet_sampling_rate = 3000
            if sflow_obj.state.packet_sampling_rate != 3000:
                raise Exception("packet_sampling_rate value not reflected in state for {}".format(sflow), False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[15].split("=")
            if (int(a[1]) != 3000):
                raise Exception(
                    "Assigned packet_sampling_rate value not reflected in state for {} in hsflowd file".format(sflow),
                    False)
        return True

    def verify_sampling_rate_modification(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.packet_sampling_rate = 3000
            if sflow_obj.state.packet_sampling_rate != 3000:
                raise Exception("packet_sampling_rate value setting to 20 not reflected in state for {}".format(sflow),
                                False)

        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.packet_sampling_rate = 40000
            if sflow_obj.state.packet_sampling_rate != 40000:
                raise Exception(
                    "packet_sampling_rate value modification to 30 not reflected in state for {}".format(sflow), False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[15].split("=")
            if (int(a[1]) != 40000):
                raise Exception(
                    "default_packet_sampling_rate value not reflected in state for {} in hsflowd file".format(sflow),
                    False)
        return True

    def verify_sampling_rate_deletion(self, sflowlist, switchlist, default_packet_sampling_rate):
        commands = ["config", "no sflow global packet-sampling-rate", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)
        self.rsleep(1)

        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            if sflow_obj.state.packet_sampling_rate != default_packet_sampling_rate:
                raise Exception("default_packet_sampling_rate not reflected in state for {}".format(sflow), False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[15].split("=")
            if (int(a[1]) != default_packet_sampling_rate):
                raise Exception(
                    "default_packet_sampling_rate value not reflected in state for {} in hsflowd file".format(sflow),
                    False)
        return True

    def configure_collector_ip_address(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            self.rsleep(2)
            # sflow_obj.config.ipv4.collector = [{"address": "10.9.195.11", "port": 6343}]
            col_list = sflow_obj.state.ipv4.get_collector()
            for col in col_list:
                if col._address != u'10.9.195.11' or col._port != 6343:
                    raise Exception(
                        " For {} Collector address expected: {} received: {}. Collector port value expected: {} received: {}".format(
                            sflow, "10.9.195.11", col._address, 6343, col._port))

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            hsflowd_col = contents_of_hsflowd[18].strip(" ")
            if hsflowd_col != "collector { ip=10.9.195.11 udpport=6343 }":
                raise Exception(
                    "hsflowd file expected: collector { ip=10.9.195.11 udpport=6343 } received {}".format(hsflowd_col),
                    False)
        return True

    def delete_collector_ip_address(self, switchlist, sflowlist):
        try:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "no sflow global ipv4 collector 10.9.195.11", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)

                filename = "/etc/hsflowd.conf"
                command = "cat {}".format(filename)
                file_contents = switch_obj.run_ssh_command(command)
                contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
                hsflowd_col = contents_of_hsflowd[18].strip(" ")
                if hsflowd_col != 'collector { ip=127.0.0.1 udpport=6343 }':
                    raise Exception(
                        "hsflowd file expected: collector { ip=127.0.0.1 udpport=6343 } received {}".format(
                            hsflowd_col),
                        False)

            for sflow in sflowlist:
                sflow_obj = self.tfw.get_object(sflow)
                col_list = sflow_obj.state.ipv4.get_collector()
                if not col_list:
                    raise Exception(
                        "Default values for collector not restored in {}. Collector list is deleted.".format(sflow))
                for col in col_list:
                    if col._address != u'127.0.0.1' or col._port != 6343:
                        raise Exception(
                            " For {} Collector address expected: {} received: {}. Collector port value expected: {} received: {}".format(
                                sflow, "127.0.0.1", col._address, 6343, col._port))
        finally:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "sflow global ipv4 collector 10.9.195.11", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)
        return True

    def configure_collector_ip_address_l4_port(self, sflowlist, ipv4Collectors):
        for ipv4Collector in ipv4Collectors:
            ipv4Collector_obj = self.tfw.get_object(ipv4Collector)
            ipv4Collector_obj.port = 7000
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            col_list = sflow_obj.state.ipv4.get_collector()

            for col in col_list:
                if col._port != 7000:
                    raise Exception(
                        " For {} Collector port value expected: {} received: {}".format(
                            sflow, 7000, col._port))

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            hsflowd_col = contents_of_hsflowd[18].strip(" ")
            if hsflowd_col != "collector { ip=10.9.195.11 udpport=7000 }":
                raise Exception(
                    "hsflowd file expected: collector { ip=10.9.195.11 udpport=7000 } received {}".format(hsflowd_col),
                    False)
        return True

    def modification_collector_ip_address_l4_port(self, sflowlist, ipv4Collectors):
        for ipv4Collector in ipv4Collectors:
            ipv4Collector_obj = self.tfw.get_object(ipv4Collector)
            ipv4Collector_obj.port = 8000

        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            col_list = sflow_obj.state.ipv4.get_collector()

            for col in col_list:
                if col._port != 8000:
                    raise Exception(
                        " For {} Collector port value expected: {} received: {}".format(
                            sflow, 8000, col._port))

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            hsflowd_col = contents_of_hsflowd[18].strip(" ")
            if hsflowd_col != "collector { ip=10.9.195.11 udpport=8000 }":
                raise Exception(
                    "hsflowd file expected: collector { ip=10.9.195.11 udpport=8000 } received {}".format(hsflowd_col),
                    False)
        for ipv4Collector in ipv4Collectors:
            ipv4Collector_obj = self.tfw.get_object(ipv4Collector)
            ipv4Collector_obj.port = 6343

        self.rsleep(2)

        return True

    def deletion_collector_ip_address_l4_port(self, switchlist, sflowlist):
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            commands = ["config", "sflow global ipv4 collector 10.9.195.11", "no port", "commit", "end"]
            response = switch_obj.run_cli_commands(commands)

            filename = "/etc/hsflowd.conf"
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            hsflowd_col = contents_of_hsflowd[18].strip(" ")
            if hsflowd_col != 'collector { ip=10.9.195.11 udpport=6343 }':
                raise Exception(
                    "hsflowd file expected: collector { ip=10.9.195.11 udpport=6343 } received {}".format(hsflowd_col),
                    False)

        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            col_list = sflow_obj.state.ipv4.get_collector()
            if not col_list:
                raise Exception(
                    "Default values for collector not restored in {}. Collector list is deleted.".format(sflow))
            for col in col_list:
                if col._address != u'10.9.195.11' or col._port != 6343:
                    raise Exception(
                        " For {} Collector address expected: {} received: {}. Collector port value expected: {} received: {}".format(
                            sflow, "10.9.195.11", col._address, 6343, col._port))
        return True

    def verify_source_interface(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.source_interface = "ma1"
            if str(sflow_obj.state.source_interface) != "ma1":
                raise Exception(
                    "source_interface value not reflected in state for {}. Expected: ma1 Received: {}".format(sflow,
                                                                                                              str(
                                                                                                                  sflow_obj.state.source_interface)),
                    False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[8].split("= ")
            if (str(a[1]) != 'ma1'):
                raise Exception(
                    "source_interface value not reflected in state for {} in hsflowd file. Expected: ma1 Received: {}".format(
                        sflow, str(a[1])),
                    False)
        return True

    def modify_source_interface(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.source_interface = "swp3"
            if str(sflow_obj.state.source_interface) != "swp3":
                raise Exception(
                    "source_interface value not reflected in state for {}. Expected: swp3 Received: {}".format(sflow,
                                                                                                               str(
                                                                                                                   sflow_obj.state.source_interface)),
                    False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[8].split("= ")
            if (str(a[1]) != 'swp3'):
                raise Exception(
                    "source_interface value not reflected in state for {} in hsflowd file. Expected: swp3 Received: {}".format(
                        sflow, str(a[1])),
                    False)
        return True

    def delete_source_interface(self, switchlist, sflowlist):
        try:

            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "no sflow global source-interface", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)
                Logger.step("Deleting sflow global source-interface")

                filename = "/etc/hsflowd.conf"
                command = "cat {}".format(filename)
                file_contents = switch_obj.run_ssh_command(command)
                self.rsleep(5)
                contents_of_hsflowd = file_contents.strip(" ").split("\r\n")

                if "agent = ma1" in contents_of_hsflowd:
                    raise Exception(
                        "agent = ma1 not deleted from hsflowd file", False)

            for sflow in sflowlist:
                sflow_obj = self.tfw.get_object(sflow)
                if sflow_obj.state.source_interface:
                    raise Exception("source_interface not deleted", False)
        finally:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "sflow global source-interface ma1", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)

        return True

    def enable_sflow(self, sflowIntflist):
        for sflow in sflowIntflist:
            sflow_obj = self.tfw.get_object(sflow)

            if str(sflow_obj._direction) != "ingress":
                raise Exception(
                    "direction value not reflected in state for {}. Expected: ingress Received: {}".format(sflow, str(
                        sflow_obj._direction)), False)
            if int(sflow_obj._packet_sampling_rate) != 38:
                raise Exception(
                    "packet_sampling_rate value not reflected in state for {}. Expected: 38 Received: {}".format(sflow,
                                                                                                                 str(
                                                                                                                     sflow_obj._packet_sampling_rate)),
                    False)
            if str(sflow_obj._name) != "ma1":
                raise Exception("name value not reflected in state for {}. Expected: ma1 Received: {}".format(sflow,
                                                                                                              str(
                                                                                                                  sflow_obj._name)),
                                False)
        return True

    def disable_sflow(self, switchlist, sflowlist):
        try:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "no sflow", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)

            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                stopped = self.wait_for_hsflowd_stop(switch_obj)
                if stopped:
                    Logger.step("service hsflowd stopped")
                else:
                    Logger.step("service hsflowd did not stop for {}".format(switch))
                    raise Exception("service hsflowd did not stop for {}".format(switch))

        finally:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "sflow interface swp7 ingress", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)
                commands = ["config", "sflow interface ma1 ingress", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)

        return True

    def enable_sflow_collection(self, switchlist, vm_ip_list):
        commands = ["config", "sflow global network-instance default", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)

        started = sflowCollector.start_sflow_collection("/tmp/sflowtool3")
        self.rsleep(15)
        sflowCollector.logout()
        not_allowed_ip = ["datagramSourceIP {}".format(str(vm_ip_list[0])),
                          "datagramSourceIP {}".format(str(vm_ip_list[1])),
                          "datagramSourceIP {}".format(str(vm_ip_list[2]))]
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool3")
        if len(samples_received) > 1:
            for element in samples_received[1:]:
                if 'datagramSourceIP' in element:
                    for a_i in not_allowed_ip:
                        if a_i in element:
                            raise Exception(
                                " Collector is getting samples from {} even though there is no route to collector via ma1 interface under default vrf".format(
                                    a_i))
        return True

    def enable_sflow_collection_management(self, switchlist):
        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)

        started = sflowCollector.start_sflow_collection("/tmp/sflowtool4")
        self.rsleep(75)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool4")
        for element in samples_received:
            if "disk_total" in element:
                return True
        raise Exception("Counter sample with cpu/memory stats not found.")

    def sflow_sampling_on_L3(self, sflowlist, switchlist):
        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtool4")

        self.rsleep(15)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool4")
        l1 = len(samples_received)

        started = sflowCollector.start_sflow_collection("/tmp/sflowtool4")
        self.rsleep(60)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool4")
        l2 = len(samples_received)

        if not l2 > l1:
            raise Exception("Not receiving packets")

        return True

    def change_source_ip(self, switchlist, vm_ip_list):
        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtool1")

        self.rsleep(60)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool1")
        a = 1

        # TEST CASE INCOMPLETE. WE DON'T KNOW WHERE TO LOOK TO VERIFY. HE WILL TELL ME LATER.

    def remove_source_interface(self, switchlist, vm_ip_list):
        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtool1")

        self.rsleep(30)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool1")

        commands = ["config", "no sflow global source-interface", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)
            if response == False:
                Logger.step("Could not execute 'no sflow global source-interface' command on {} ".format(switch))

        started = sflowCollector.start_sflow_collection("/tmp/sflowtool2")

        self.rsleep(60)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool2")
        allowed_ip = ["datagramSourceIP {}".format(str(vm_ip_list[0])),
                      "datagramSourceIP {}".format(str(vm_ip_list[1])),
                      "datagramSourceIP {}".format(str(vm_ip_list[2]))]

        for element in samples_received[1:]:
            if "FLOWSAMPLE" in element and 'datagramSourceIP' in element:
                for a_i in allowed_ip:
                    if a_i in element:
                        found = True
                        raise Exception(
                            " Expected no datagram on collector, found datagram with source ip in {}".format(
                                vm_ip_list))
        return True

    def multiple_collector(self, switchlist, vm_ip_list):
        commands = ["config", "sflow global network-instance management", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)
        commands = ["config", "sflow global ipv4 collector 10.9.195.11", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)
            if response == False:
                Logger.step(
                    "Could not execute 'sflow global ipv4 collector 10.9.195.11' command on {} ".format(switch))

        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)

        started = sflowCollector.start_sflow_collection("/tmp/sflowtool1")
        self.rsleep(30)
        sflowCollector.logout()
        samples_received_207 = sflowCollector.get_sflow_samples("/tmp/sflowtool1")

        ##
        self.rsleep(15)
        commands = ["config", "sflow global ipv4 collector 10.9.195.12", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)
            if not response:
                Logger.step(
                    "Could not execute 'sflow global ipv4 collector 10.9.195.12' command on {} ".format(switch))

        host = "10.9.195.12"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtool1")
        self.rsleep(30)
        sflowCollector.logout()
        samples_received_210 = sflowCollector.get_sflow_samples("/tmp/sflowtool1")

        if len(samples_received_207) < 2:
            raise Exception("Samples are not being collected on 10.9.195.11")
        if len(samples_received_210) < 2:
            raise Exception("Samples are not being collected on 10.9.195.12")

        return True

    def increase_counter_sample_interval(self, switchlist):
        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtool4")

        self.rsleep(60)
        sflowCollector.logout()
        samples_received_60 = sflowCollector.get_sflow_samples("/tmp/sflowtool4")

        commands = ["config", "sflow global counter-sampling-interval 10", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtool5")

        self.rsleep(60)
        sflowCollector.logout()
        samples_received_10 = sflowCollector.get_sflow_samples("/tmp/sflowtool5")

        if len(samples_received_10) < 2 * len(samples_received_60):
            raise Exception(
                "Received {} samples with counter-sampling-interval 10 and {} samples with counter-sampling-interval 60.".format(
                    len(samples_received_10), len(samples_received_60)))
        return True

    def vm_as_collector(self, switchlist, ip):
        commands = ["config", "sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = ip
        user = "root"
        password = "onl"

        # PKILL sflowtool on host here

        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        for i in range(0, 4):
            result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/usr/local/bin/sflowsamples")

        self.rsleep(60)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/usr/local/bin/sflowsamples")
        if not samples_received:
            raise Exception("No samples were received")
        if not isinstance(samples_received, list):
            raise Exception("Expected list of samples. Received : {}".format(samples_received))
        if len(samples_received) < 2:
            raise Exception("Expected list of samples. Received : {}".format(samples_received))

        return True

    def remove_vm_as_collector(self, switchlist, ip, vm_ip_list):
        commands = ["config", "no sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = ip
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/usr/local/bin/sflowsamples")

        self.rsleep(60)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/usr/local/bin/sflowsamples")

        forbidden_ip_list = ["datagramSourceIP {}".format(str(vm_ip_list[0])),
                             "datagramSourceIP {}".format(str(vm_ip_list[1])),
                             "datagramSourceIP {}".format(str(vm_ip_list[2]))]

        for element in samples_received[1:]:
            if 'datagramSourceIP' in element:
                for a_i in forbidden_ip_list:
                    if a_i in element:
                        raise Exception(
                            "Received samples from {} even though collector is not configured in that switch".format(
                                a_i))
        return True

    def stop_spyder(self, switchlist):
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            switch_obj.restart_arcos()

        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_ssh_command("service hsflowd status")
            if "hsflowd is running." not in response:
                raise Exception("hsflowd service did not restart after spyder restart in {}".format(switch))
        return True

    def restart_hsflowd(self, switchlist):
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            switch_obj.run_ssh_command("service hsflowd stop")
            self.rsleep(1)
            switch_obj.run_ssh_command("service hsflowd start")
            self.rsleep(2)

        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_ssh_command("service hsflowd status")
            if "hsflowd is running." not in response:
                raise Exception("hsflowd service did not restart after spyder restart in {}".format(switch))

        return True

    def restart_sflow(self, switchlist, ip, nwname):
        commands = ["config", "sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            switch_obj.run_cli_commands(commands)

        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_ssh_command("service hsflowd status")
            if "hsflowd is running." not in response:
                Logger.step("hsflowd service is not running before killing it in {}".format(switch))
                switch_obj.run_ssh_command("service hsflowd start")
                self.rsleep(3)

        commands = "pkill sflow.bin"
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_ssh_command(commands)

        self.rsleep(5)

        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_ssh_command('ps -ef | grep "sflow.bin" | grep -v "grep"')
            if "/usr/lib/arcos/sflow.bin" not in response:
                nw = self.tfw.get_object(nwname)
                nw.delete()
                raise Exception("sflow.bin process did not restart automatically after killing it in {}".format(switch))

        return True

    def sflow_on_ma1(self, switchlist, vm_ip_list, ip):
        commands = ["config", "sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = ip
        user = "root"
        password = "onl"
        # pkill sflow here
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/usr/local/bin/sflowsamples38")

        self.rsleep(60, "Wait to collect samples")
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/usr/local/bin/sflowsamples38")

        allowed_ip = ["datagramSourceIP {}".format(str(vm_ip_list[0])),
                      "datagramSourceIP {}".format(str(vm_ip_list[1])),
                      "datagramSourceIP {}".format(str(vm_ip_list[2]))]

        for element in samples_received[1:]:
            if 'datagramSourceIP' in element:
                for a_i in allowed_ip:
                    if a_i in element:
                        return True

        raise Exception("expected datagram SourceIP in {} not found".format(vm_ip_list))

    def sflow_on_swp_interface(self, switchlist, allowed_ip_ma1, allowed_ip_swp, ip, intf_lists):


        for i, switch in enumerate(switchlist):
            commands = ["config", "sflow global source-interface {}".format(intf_lists[i]), "commit", "end"]
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow global network-instance management", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow global packet-sampling-rate 100", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow interface swp7 ingress packet-sampling-rate 43", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = ip
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)

        started = sflowCollector.start_sflow_collection("/usr/local/bin/sflowsamples39")

        self.rsleep(45)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/usr/local/bin/sflowsamples39")

        allowed_ip_ma1 = ["datagramSourceIP {}".format(str(allowed_ip_ma1[0])),
                          "datagramSourceIP {}".format(str(allowed_ip_ma1[1])),
                          "datagramSourceIP {}".format(str(allowed_ip_ma1[2]))]

        allowed_ip_swp = ["agent {}".format(str(allowed_ip_swp[0])),
                          "agent {}".format(str(allowed_ip_swp[1])),
                          "agent {}".format(str(allowed_ip_swp[2])),
                          "agent {}".format(str(allowed_ip_swp[3])),
                          ]

        found_ma1 = False
        found_swp = False
        for element in samples_received[1:]:
            if 'datagramSourceIP' in element:
                for a_i in allowed_ip_ma1:
                    if a_i in element:
                        found_ma1 = True

                for a_i in allowed_ip_swp:
                    if a_i in element:
                        found_swp = True

        if not found_ma1:
            raise Exception("No datagram found with source as ma1 interface.")
        if not found_swp:
            raise Exception("No datagram found with source as swp interface.")
        return True

    def disable_sflow_on_ma1_interface(self, switchlist, allowed_ip_ma1, allowed_ip_swp, ip):
        commands = ["config", "sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "no sflow interface ma1 ingress", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = ip
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/usr/local/bin/sflowsamples1")

        self.rsleep(30)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/usr/local/bin/sflowsamples1")

        allowed_ip_ma1 = ["datagramSourceIP {}".format(str(allowed_ip_ma1[0])),
                          "datagramSourceIP {}".format(str(allowed_ip_ma1[1])),
                          "datagramSourceIP {}".format(str(allowed_ip_ma1[2]))]

        allowed_ip_swp = ["agent {}".format(str(allowed_ip_swp[0])),
                          "agent {}".format(str(allowed_ip_swp[1])),
                          "agent {}".format(str(allowed_ip_swp[2])),
                          "agent {}".format(str(allowed_ip_swp[3])),
                          ]

        found_ma1_without_swp = False
        for element in samples_received[1:]:
            if 'datagramSourceIP' in element:
                for a_i in allowed_ip_ma1:
                    if a_i in element:
                        for a_swp in allowed_ip_swp:
                            if allowed_ip_swp[0] not in element and allowed_ip_swp[1] not in element and allowed_ip_swp[
                                2] not in element and allowed_ip_swp[3] not in element:
                                found_ma1_without_swp = True

        if found_ma1_without_swp:
            raise Exception("Datagram found with source as ma1 interface. Expecting only swp interfaces as source.")

        return True

    def disable_sflow_on_swp_interface(self, switchlist, allowed_ip_ma1, allowed_ip_swp, ip):

        commands = ["config", "sflow global ipv4 collector {}".format(ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "no sflow interface swp7 ingress", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        host = ip
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)
        started = sflowCollector.start_sflow_collection("/usr/local/bin/sflowsamples1")

        self.rsleep(30)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/usr/local/bin/sflowsamples1")

        # configuring back so that teardown does not fail
        commands = ["config", "sflow interface swp7 ingress", "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        allowed_ip_ma1 = ["datagramSourceIP {}".format(str(allowed_ip_ma1[0])),
                          "datagramSourceIP {}".format(str(allowed_ip_ma1[1])),
                          "datagramSourceIP {}".format(str(allowed_ip_ma1[2]))]

        allowed_ip_swp = ["datagramSourceIP {}".format(str(allowed_ip_swp[0])),
                          "datagramSourceIP {}".format(str(allowed_ip_swp[1])),
                          "datagramSourceIP {}".format(str(allowed_ip_swp[2])),
                          "datagramSourceIP {}".format(str(allowed_ip_swp[3])),
                          ]

        found_ma1 = False
        found_swp = False
        for element in samples_received[1:]:
            if 'datagramSourceIP' in element:
                for a_i in allowed_ip_ma1:
                    if a_i in element:
                        found_ma1 = True

                for a_i in allowed_ip_swp:
                    if a_i in element:
                        found_swp = True

        if found_swp:
            raise Exception("Datagram found with source as swp interface. Expecting only ma1 interfaces as source.")

        return True

    def disable_sflow_on_ma1_and_swp_interface(self, switchlist, allowed_ip_ma1, allowed_ip_swp):
        try:
            commands = ["config", "no sflow interface swp7 ingress", "commit", "end"]
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                response = switch_obj.run_cli_commands(commands)
            commands = ["config", "no sflow interface ma1 ingress", "commit", "end"]
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                response = switch_obj.run_cli_commands(commands)

            self.rsleep(5)
            host = "10.9.195.11"
            user = "root"
            password = "onl"
            sflowCollector = SflowCollector(host, user=user, password=password)
            command = "pkill sflowtool"
            result = sflowCollector.run_ssh_command(command)
            started = sflowCollector.start_sflow_collection("/tmp/sflowtool6")

            self.rsleep(30)
            sflowCollector.logout()
            samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool6")

            allowed_ip_ma1 = ["datagramSourceIP {}".format(str(allowed_ip_ma1[0])),
                              "datagramSourceIP {}".format(str(allowed_ip_ma1[1])),
                              "datagramSourceIP {}".format(str(allowed_ip_ma1[2]))]

            allowed_ip_swp = ["datagramSourceIP {}".format(str(allowed_ip_swp[0])),
                              "datagramSourceIP {}".format(str(allowed_ip_swp[1])),
                              "datagramSourceIP {}".format(str(allowed_ip_swp[2])),
                              "datagramSourceIP {}".format(str(allowed_ip_swp[3])),
                              ]

            found_ma1 = False
            found_swp = False

            for element in samples_received[1:]:
                if 'datagramSourceIP' in element:
                    for a_i in allowed_ip_ma1:
                        if a_i in element and "FLOWSAMPLE" in element:
                            found_ma1 = True

            if found_ma1:
                raise Exception("Received samples. Expecting no samples.")
        finally:
            commands = ["config", "sflow interface swp7 ingress", "commit", "end"]
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                response = switch_obj.run_cli_commands(commands)
            commands = ["config", "sflow interface ma1 ingress", "commit", "end"]
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                response = switch_obj.run_cli_commands(commands)

        return True

    def configure_collector_ip_address_v6(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            self.rsleep(5)
            col_list = sflow_obj.state.ipv6.get_collector()

            if not col_list:
                raise Exception("Collector object not found")
            for col in col_list:
                if col._address != u'172:16:2::2' or col._port != 6343:
                    raise Exception(
                        " For {} Collector address expected: {} received: {}. Collector port value expected: {} received: {}".format(
                            sflow, "172:16:2::2", col._address, 6343, col._port))

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            self.rsleep(2)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            hsflowd_col = contents_of_hsflowd[18].strip(" ")
            if hsflowd_col != "collector { ip=172:16:2::2 udpport=6343 }":
                raise Exception(
                    "hsflowd file expected: collector { ip=172:16:2::2 udpport=6343 } received {}".format(hsflowd_col),
                    False)
        return True

    def delete_collector_ip_address_v6(self, switchlist, sflowlist):
        try:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "no sflow global ipv6 collector 172:16:2::2", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)

                self.rsleep(10)

                filename = "/etc/hsflowd.conf"
                command = "cat {}".format(filename)
                file_contents = switch_obj.run_ssh_command(command)
                self.rsleep(5)
                contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
                hsflowd_col = contents_of_hsflowd[18].strip(" ")
                if hsflowd_col != 'collector { ip=127.0.0.1 udpport=6343 }':
                    raise Exception(
                        "hsflowd file expected: collector { ip=127.0.0.1 udpport=6343 } received {}".format(
                            hsflowd_col),
                        False)
        finally:
            for switch in switchlist:
                switch_obj = self.tfw.get_object(switch)
                commands = ["config", "sflow global ipv6 collector 172:16:2::2", "commit", "end"]
                response = switch_obj.run_cli_commands(commands)
        return True

    def configure_collector_ip_address_l4_port_v6(self, sflowlist, ipv6Collectors):
        try:
            for ipv6Collector in ipv6Collectors:
                ipv6Collector_obj = self.tfw.get_object(ipv6Collector)
                ipv6Collector_obj._port = 7000
            self.rsleep(5)
            for sflow in sflowlist:
                sflow_obj = self.tfw.get_object(sflow)
                col_list = sflow_obj.state.ipv6.get_collector()

                for col in col_list:
                    if col._port != 7000:
                        raise Exception(
                            " For {} Collector port value expected: {} received: {}".format(
                                sflow, 7000, col._port))

                filename = "/etc/hsflowd.conf"
                switchname = sflow.split(":")[0]
                switch_obj = self.tfw.get_object(switchname)
                command = "cat {}".format(filename)
                file_contents = switch_obj.run_ssh_command(command)
                contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
                hsflowd_col = contents_of_hsflowd[18].strip(" ")
                if hsflowd_col != "collector { ip=172:16:2::2 udpport=7000 }":
                    raise Exception(
                        "hsflowd file expected: collector { ip=172:16:2::2 udpport=7000 } received {}".format(
                            hsflowd_col),
                        False)
        finally:
            for ipv6Collector in ipv6Collectors:
                ipv6Collector_obj = self.tfw.get_object(ipv6Collector)
                ipv6Collector_obj.port = 6343
        return True

    def enable_sflow_egress(self, sflowIntflist, sampling_rate=39, swp_intf1="swp7", swp_intf2="swp7"):

        intf_values = [swp_intf1, swp_intf2]

        for i, sflow in enumerate(sflowIntflist):
            sflow_obj = self.tfw.get_object(sflow)

            if str(sflow_obj._direction) != "egress":
                raise Exception(
                    "direction value not reflected in state for {}. Expected: egress Received: {}".format(sflow, str(
                        sflow_obj._direction)), False)
            if int(sflow_obj._packet_sampling_rate) != sampling_rate:
                raise Exception("packet_sampling_rate value not reflected in state for {}.Received: {}".format(sflow,sflow_obj._packet_sampling_rate), False)
            if str(sflow_obj._name) != intf_values[i]:
                raise Exception("name value not reflected in state for {}. Received: {}".format(sflow,sflow_obj._name),False)
        return True

    def delete_subintf(self, objects):
        for object in objects:
            switchname = object.split(":")[0]
            intfname = object.split(":")[1]
            switch_obj = self.tfw.get_object(switchname)
            intf_obj = self.tfw.get_object(intfname)
            commands = ["config", "interface {}".format(intfname), "no subinterface 0", "commit", "end"]

            response = switch_obj.run_cli_commands(commands)
            if ("Commit complete") not in response:
                Logger.step("Could not delete subinterface 0")

    def l2_port(self, sflowlist, ipv4Collectors):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)

        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtoolvlan")

        self.rsleep(30)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtoolvlan")

        for element in samples_received[1:]:
            if "sampleType FLOWSAMPLE" in element:
                if "in_vlan 110" not in element or "extendedType SWITCH" not in element:
                    raise Exception(" Expected in_vlan 110 in FLOWSAMPLE. Not found.")
        return True

    def validate_each_field(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)

        host = "10.9.195.11"
        user = "root"
        password = "onl"
        sflowCollector = SflowCollector(host, user=user, password=password)
        started = sflowCollector.start_sflow_collection("/tmp/sflowtoolsamples")

        self.rsleep(30)
        sflowCollector.logout()
        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtoolsamples")

        for element in samples_received[1:]:
            if "sampleType FLOWSAMPLE" in element:
                if "in_vlan 110" not in element or "extendedType SWITCH" not in element:
                    raise Exception(" Expected in_vlan 110 in FLOWSAMPLE. Not found.")
        return True

    def configure_source_loopback_interface(self, sflowlist):
        for sflow in sflowlist:
            sflow_obj = self.tfw.get_object(sflow)
            sflow_obj.config.source_interface = "loopback0"
            if str(sflow_obj.state.source_interface) != "loopback0":
                raise Exception(
                    "source_interface value not reflected in state for {}. Expected: loobpack0 Received: {}".format(sflow,
                                                                                                              str(
                                                                                                                  sflow_obj.state.source_interface)),
                    False)

            filename = "/etc/hsflowd.conf"
            switchname = sflow.split(":")[0]
            switch_obj = self.tfw.get_object(switchname)
            command = "cat {}".format(filename)
            file_contents = switch_obj.run_ssh_command(command)
            contents_of_hsflowd = file_contents.strip(" ").split("\r\n")
            a = contents_of_hsflowd[8].split("= ")
            if (str(a[1]) != 'loopback0'):
                raise Exception(
                    "source_interface value not reflected in state for {} in hsflowd file. Expected: ma1 Received: {}".format(
                        sflow, str(a[1])),
                    False)
        return True

    def enable_sflow_on_interface(self, switchlist, network_instance, sampling_rate, collector_ip, source_intf_list, intf, dir):

        for i, switch in enumerate(switchlist):
            commands = ["config", "sflow global source-interface {}".format(source_intf_list[i]), "commit", "end"]
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow global network-instance {}".format(network_instance), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow global packet-sampling-rate {}".format(sampling_rate), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow global ipv4 collector {}".format(collector_ip), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        commands = ["config", "sflow interface {} {} packet-sampling-rate 10".format(intf, dir), "commit", "end"]
        for switch in switchlist:
            switch_obj = self.tfw.get_object(switch)
            response = switch_obj.run_cli_commands(commands)

        return True

    def verify_source_loopback_interface(self, loopback_ipv4_list, c_ip, user, password):

        sflowCollector = SflowCollector(c_ip, user=user, password=password)
        command = "pkill sflowtool"
        result = sflowCollector.run_ssh_command(command)

        started = sflowCollector.start_sflow_collection("/tmp/sflowtool_loopback")
        self.rsleep(15)
        sflowCollector.logout()

        agentIPs = []

        for ip_addr in loopback_ipv4_list:
            agentIPs.append("agent {}".format(str(ip_addr)))

        samples_received = sflowCollector.get_sflow_samples("/tmp/sflowtool_loopback")
        if len(samples_received) > 1:
            for element in samples_received[1:]:
                if 'agent ' in element:
                    count = 0
                    for a_i in agentIPs:
                        if a_i in element:
                            count = count + 1
                    if count == 0:
                        raise Exception(
                            "source_interface value not reflected . Expected: loopback0",
                            False)
        else:
            raise Exception("No samples found in /tmp/sflowtool_loopback", False)
        return True
