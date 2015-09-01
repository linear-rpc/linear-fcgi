#!/bin/sh

IFCONFIG=`which ifconfig`
hwaddrs=`${IFCONFIG} | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'`
result=`./_uuid_node_test 2>&1 >/dev/null`
flag=0

echo "result: ${result}"
for hwaddr in ${hwaddrs}; do
    echo "check : ${hwaddr}"
    if [ ${hwaddr} = ${result} ]; then
        flag=1
        break;
    fi
done

if [ ${flag} -ne 1 ]; then
    echo 'not found valid macaddr'
    exit 1
fi

exit 0
