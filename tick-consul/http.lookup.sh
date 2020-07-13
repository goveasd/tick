echo "*** List of services ***"
curl http://localhost:8500/v1/catalog/services?pretty

echo "*** Drill down into web services ***"
curl http://localhost:8500/v1/catalog/service/web?pretty

echo "*** Health web services ***"
curl http://localhost:8500/v1/health/service/web?pretty
