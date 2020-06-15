const {TickRequest, TickDelta} = require('./tick_pb.js');
const {TobClient} = require('./tick_grpc_web_pb.js');

var client = new TobClient('http://localhost:9001');

var request = new TickRequest();
request.setSymbol('IBM');

/*
for (let step = 0; step < 1; step++) {
	setTimeout(function() {
		client.CurrentPrice(request, {}, (err, response) => {
  		console.log(response.getMessage());
		});
	}, (2 * 1000));
}
*/


client.currentPrice(request, {}, (err, response) => {
		if (response != null) {
			console.log("Response Type:");
			console.log(typeof response);
		}
		else {
			console.log("Response is null");
		}
		if (err != null) {
			console.log("Err Type:");
			console.log(typeof err);
		}
		else {
			console.log("err is null");
		}

	if ( response != null && typeof response !== 'undefined' ) {
		console.log(response.getDeltaPrice());
		console.log(response.getDeltaSize());
	}
});
