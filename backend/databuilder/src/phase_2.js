// args: 
//      - current layer folders list
//      - place to write next layer
const fs = require('fs')
const path = require('path');
const { exec, spawn } = require("child_process");

var file_size_threshold = 8 * 1000 * 1000

var new_paths = ''


function exitHandler(options, exitCode) {
   fs.writeFileSync(process.argv[3], new_paths);
}

// do something when app is closing
process.on('exit', exitHandler.bind(null,{cleanup:true}));


function work_file(c_path, c_file){
    console.log(`Starting working for file ${c_path}${c_file}!`)
    let base_filename = c_file.split('.raw')[0]
    let new_dir = c_path + base_filename + '/'
    new_paths += new_dir + '\n'
    if (!fs.existsSync(new_dir))
        fs.mkdirSync(new_dir)

    exec(`bash ./exe.sh ${c_path} ${base_filename} &>>../.log/phase_2_cpp`, function (e, out, stderr) {
        if (e) {
            console.log(e)
        }
        console.log(`${c_path}${c_file} finished processing!`)
    })

}

function work_folder(c_path) {
    if (c_path.length < 2)
        return
    console.log(`Starting to work on path: ${c_path}`)

    fs.readdir(c_path, (err, files) => {
        files.forEach(file => {
            if (!file.includes('.raw'))
                return;
            var stats = fs.statSync(path.join(c_path,file))
            var size = stats.size;
            if(size < file_size_threshold)  
                return;
            work_file(c_path, file)

        });
    });
}


folders = fs.readFileSync(process.argv[2], { encoding: 'utf8', flag: 'r' })
folders = folders.split('\n')
folders.forEach(element => {
    work_folder(element)
});
