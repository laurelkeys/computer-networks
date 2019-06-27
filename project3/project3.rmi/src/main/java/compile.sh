#!/usr/bin/env bash
sh ./clean.sh
javac -source 8 -target 8 utils/DataKeeper.java Client.java Server.java
echo "Usage:"
echo " 1. Start a remote object registry:"
echo "  >>> rmiregistry [PORT_NUMBER] &"
echo " 2. Run the server:"
echo "  >>> java Server [-p PORT_NUMBER] [-a ADDRESS]"
echo " 3. Run the client:"
echo "  >>> java Client [-p PORT_NUMBER] [-a ADDRESS]"
echo "Notes: "
echo " - default PORT_NUMBER is 1099"
echo " - default ADDRESS is localhost"
echo " - you should wait a few seconds after running rmiregistry"
echo "   before starting the server, or it may raise a ConnectException"
