echo "** Stopping web service web1 **"
consul exec -node web1 docker stop web

echo "** Stopping web service web2 **"
consul exec -node web2 docker stop web

echo "** Stopping web service web3 **"
consul exec -node web3 docker stop web
