const {TickRequest, TickDelta} = require('./tick_pb.js');
const {TobClient} = require('./tick_grpc_web_pb.js');

var client = new TobClient('http://localhost:9001');

var request = new TickRequest();
request.setSymbol('IBM');

getDelta();
for (let step = 0; step < 10; step++) {
	setTimeout(function() {
  		getDelta();
	}, (20 * 1000));
}


function getDelta() {
	client.currentPrice(request, {}, (err, response) => {

		if ( response != null && typeof response !== 'undefined' ) {
			console.log(response.getDeltaPrice());
			console.log(response.getDeltaSize());

			document.getElementById("Price").value = response.getDeltaPrice();
			document.getElementById("Size").value = response.getDeltaSize();
			document.getElementById("Status").value = "OK";

		} else {
			console.log("Err Type:");
			console.log(typeof err);
			document.getElementById("Status").value = "Error";
		}
	});
}
