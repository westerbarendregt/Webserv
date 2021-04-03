#!/bin/bash
siege -c20 -r500 -d0.1 -v -f urls.txt
