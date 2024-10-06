const fs = require("fs");
const { parse } = require("csv-parse");

var args = process.argv.slice(2);

fs.createReadStream(args[0])
  .pipe(parse({ delimiter: ",", from_line: 2 }))
  .on("data", function (row) {
    // console.log(row);
    console.log(`${row[1]} ${row[3]} ${row[4]} ${row[5]} ${row[9]} ${row[10]} ${row[14]} ${row[15]} ${row[16]}`)
    // exit(0);
  })
