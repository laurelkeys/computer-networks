#!/usr/bin/env bash
sh clean.sh
javac -source 8 -target 8 utils/DataKeeper.java Client.java Server.java
echo "Run server:"
echo " >>> rmiregistry PORT_NUMBER &"
echo " >>> java Server.class"
echo "Run client:"
echo " >>> java Client.class"