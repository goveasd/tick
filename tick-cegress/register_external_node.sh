#!/bin/bash

#curl --request PUT --data @direction.legacyservice.json 172.20.20.41:8500/v1/catalog/register
curl --request PUT --data @direction.legacyservice.json 172.20.20.51:8500/v1/catalog/register
