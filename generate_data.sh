#!/bin/bash

red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
blue=`tput setaf 4`
lightb=`tput setaf 6`
reset=`tput sgr0`
bold=`tput bold`
rev=`tput smso`

# ---- Main ----

# Check if exist network directory with uuid
if [[ ! -d ./network ]]; then
    echo "'network' directory not found! Creating." 
    net_uuid=`uuidgen`
    mkdir -p ./network/${net_uuid}
fi

for net_uuid in ./network/*/ ; do
    echo "Network has UUID: ${net_uuid}"
done

command -v uuidgen >/dev/null 2>&1 || { echo "${red} I require foo but it's not installed.  Aborting." >&2; exit 1; }

dev_uuid=`uuidgen`

echo "Creating new device with UUID: ${dev_uuid}"

echo ""
echo "${yellow}------- Enter device info -----${reset}"
echo ""
echo -ne "${bold}name=${reset}"; read dev_name
echo -ne "${bold}manufacturer=${reset}"; read manufacturer 
echo -ne "${bold}product=${reset}"; read product 
echo -ne "${bold}version=${reset}"; read version 
echo -ne "${bold}serial=${reset}"; read serial 
echo -ne "${bold}protocol=${reset}"; read protocol 
echo -ne "${bold}communication=${reset}"; read communication 

device_dir="${net_uuid}/device/${dev_uuid}"
mkdir -p ${device_dir} 

echo -e "{ 
    \"id\": \"${dev_uuid}\",
    \"name\": \"${dev_name}\",
    \"manufacturer\": \"${manufacturer}\",
    \"product\": \"${product}\",
    \"version\": \"${version}\",
    \"serial\": \"${serial}\",
    \"protocol\": \"${protocol}\",
    \"communication\": \"${communication}\",
    \"included\": false 
}" >> ${device_dir}/device.json

device=`cat ${device_dir}/device.json`
echo $device

echo ""
value_uuid=`uuidgen`
echo "Creating new value with UUID: ${value_uuid}"

echo "${yellow}------- Enter value info -----${reset}"
echo ""
echo -ne "${bold}name=${reset}"; read value_name
echo -ne "${bold}permission=${reset}"; read permission 
echo -ne "${bold}type=${reset}"; read value_type 
echo -ne "${bold}min=${reset}"; read min 
echo -ne "${bold}max=${reset}"; read max 
echo -ne "${bold}step=${reset}"; read step 

value_dir="${device_dir}/value/${value_uuid}"
mkdir -p ${value_dir} 

echo -e "{ 
    \"id\": \"${value_uuid}\",
    \"name\": \"${value_name}\",
    \"permission\": \"${permission}\",
    \"type\": \"${value_type}\",
    \"min\": \"${min}\",
    \"max\": \"${max}\",
    \"step\": \"${step}\"
}" >> ${value_dir}/value.json

value=`cat ${value_dir}/value.json`
echo $value

echo ""
state_uuid=`uuidgen`
echo "Creating new state with UUID: ${state_uuid}"

echo "${yellow}------- Enter state info -----${reset}"
echo ""
echo -ne "${bold}data=${reset}"; read data 

state_dir="${value_dir}/state/${state_uuid}"
mkdir -p ${state_dir} 

echo -e "{ 
    \"id\": \"${state_uuid}\",
    \"type\": \"Report\",
    \"data\": \"${data}\"
}" >> ${state_dir}/state.json

state=`cat ${state_dir}/state.json`
echo $state















