});
clpPromise.then(function (clp) {
  self.onmessage = function (event) {
  var problem = event['data'];
    postMessage(clp.solve(problem));
  }
  postMessage({ initialized: true });
});
