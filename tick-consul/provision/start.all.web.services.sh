echo "** Starting web service web1 **"
consul exec -node web1 docker start web

echo "** Starting web service web2 **"
consul exec -node web2 docker start web

echo "** Starting web service web3 **"
consul exec -node web3 docker start web
