var info_object = function(){
  this.id;
  this.info = [];
}
var sensor_array = []

var test_a = new info_object();
for (var i = 0; i <10; i++){
	var object = new info_object();
	object.id = 'a' + i;
	object.info = [0, 1, 2, i ];
	sensor_array.push(object);
}

test_a.id = 'a';
test_a.info = [1,2,3,4];

for (var i = 0; i<sensor_array.length;i++){
	console.log("and again " + sensor_array[i]['info'][sensor_array.length-1]);
}

sensor_array[0] = test_a;
sensor_array[0]['info'].push("hello");

console.log("what up " + sensor_array[0]['info'][sensor_array.length-1]);
console.log(test_a);
console.log(sensor_array);
console.log()

  setInterval(function(){  console.log('hello') }, 2000);
