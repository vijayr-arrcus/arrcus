*** Settings ***
Documentation    Creating a test-set
Library          SflowLibrary.py
#Suite Setup      suite setup
#Suite Teardown   suite teardown
#Test Setup       Test Setup
#Test Teardown    Test Teardown


*** Test Cases ***
#### Source loopback interface ####

TC 1.41 configuration of source loopback interface
    [Tags]    1.41  skip run trial
    [Setup]   Test Setup1  BGPv4Network01
    enable sflow on interface  ${switchlist}  ${ni_mgmt}  ${sampling_rate}  ${collector_ip}  ${src_agent_ip}  ${swp1}  ${dir_in}
    configure source loopback interface  ${sflowlist}
    verify source loopback interface  ${loopback_ipv4_list}  ${collector_ip}  ${user_name}  ${password}
#    [Teardown]  Test Teardown1  BGPv4Network01

TC 1.42 Deletion of source-interface
    [Tags]    1.42  skip run trial ignore
    [Setup]   Test Setup1  BGPv4Network01
    delete source interface  ${switchlist}  ${sflowlist}
    [Teardown]  Test Teardown1  BGPv4Network01
