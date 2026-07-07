#!/usr/bin/env bash

cd /DCPLib/example/slave
./build/dcpslave &
SLAVEID=$!
echo "Slave Started."

sleep 5 &
SLEEPID=$!
echo "Sleeping for readiness..."

wait $SLEEPID
echo "Sleep over."
cd /DCPLib/example/master
./build/dcpmaster &
MASTERID=$!


wait $MASTERID
echo "Master Done."

kill $SLAVEID

echo "Slave Ended."

