#!/usr/bin/env bash
sh clean.sh
javac -source 8 -target 8 utils/DataKeeper.java Client.java Server.java
echo "Usage:"
echo " 1. Start a remote object registry:"
echo "  >>> rmiregistry [PORT_NUMBER] &"
echo " 2. Run the server:"
echo "  >>> java Server"
echo " 3. Run the client:"
echo "  >>> java Client"
echo "Note: "
echo " default PORT_NUMBER is 1099"